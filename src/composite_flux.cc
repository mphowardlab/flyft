#include "flyft/composite_flux.h"

#include <algorithm>

namespace flyft
{

void CompositeFlux::compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state)
    {
    allocate(state);

    // initialize to zeros
    const auto mesh = *state->getMesh();
    for (const auto& t : state->getTypes())
        {
        auto j = fluxes_.at(t);
        std::fill(j->begin(),j->end(),0.);
        }

    // combine
    for (const auto& o : objects_)
        {
        o->compute(grand, state);
        for (const auto& t : state->getTypes())
            {
            auto j = fluxes_.at(t)->begin();
            auto jo = o->getFlux(t)->cbegin();
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(j,jo)
            #endif
            for (int idx=0; idx < mesh.shape(); ++idx)
                {
                j(idx) += jo(idx);
                }
            }
        }
    }

}
