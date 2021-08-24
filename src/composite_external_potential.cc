#include "flyft/composite_external_potential.h"

#include <algorithm>

namespace flyft
{

void CompositeExternalPotential::potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state)
    {
    // TODO: move compute() calls on objects here, or if someone calls potential() directly this will fail

    // fill total potential with zeros and accumulate
    const auto mesh = *state->getMesh()->local();
    auto data = V->view();
    std::fill(data.begin(),data.end(),0.);
    for (const auto& o : objects_)
        {
        auto o_data = o->getDerivative(type)->const_view();
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(data,o_data)
        #endif
        for (int idx=0; idx < mesh.shape(); ++idx)
            {
            data(idx) += o_data(idx);
            }
        }
    }

bool CompositeExternalPotential::setup(std::shared_ptr<State> state, bool compute_value)
    {
    bool compute = ExternalPotential::setup(state,compute_value);

    // evaluate the potentials during the setup
    // TODO: move this to potential() once caching is in place to guard against multiple compute() calls
    for (const auto& o : objects_)
        {
        o->compute(state,compute_value);
        }

    return compute;
    }

}
