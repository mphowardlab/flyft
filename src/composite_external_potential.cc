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
        Vtmp_ = std::make_shared<Field>(mesh.shape(),mesh.buffer_shape());
        }
    else
        {
        Vtmp_->reshape(mesh.shape(),mesh.buffer_shape());
        }

    // fill total potential with zeros
    auto data = V->first();
    std::fill(V->first(),V->last(),0.);

    for (const auto& potential : objects_)
        {
        potential->potential(Vtmp_,type,state);
        auto tmp = Vtmp_->first();
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(data,tmp)
        #endif
        for (int idx=0; idx < mesh.shape(); ++idx)
            {
            data[idx] += tmp[idx];
            }
        }
    }

}
