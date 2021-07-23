#include "flyft/composite_external_potential.h"

#include <algorithm>

namespace flyft
{

void CompositeExternalPotential::potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state)
    {
    // size temporary array
    const auto mesh = *state->getMesh();
    if (!Vtmp_)
        {
        Vtmp_ = std::make_shared<Field>(mesh.layout());
        }
    else
        {
        Vtmp_->reshape(mesh.layout());
        }

    // fill total potential with zeros and accumulate
    auto data = V->begin();
    std::fill(V->begin(),V->end(),0.);
    for (const auto& potential : objects_)
        {
        potential->potential(Vtmp_,type,state);
        auto tmp = Vtmp_->cbegin();
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(data,tmp)
        #endif
        for (int idx=0; idx < mesh.shape(); ++idx)
            {
            const int self = mesh(idx);
            data[self] += tmp[self];
            }
        }
    }

}
