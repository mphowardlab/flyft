#include "flyft/composite_external_potential.h"
#include "flyft/parallel.h"

#include <algorithm>

namespace flyft
{

void CompositeExternalPotential::potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state)
    {
    // size temporary array
    auto mesh = state->getMesh();
    if (!Vtmp_)
        {
        Vtmp_ = std::make_shared<Field>(mesh->shape());
        }
    Vtmp_->reshape(mesh->shape());

    // fill total potential with zeros
    auto data = V->data();
    parallel::fill(data,mesh->shape(),0.);

    for (const auto& potential : objects_)
        {
        potential->potential(Vtmp_,type,state);
        auto tmp = Vtmp_->data();

        const auto shape = mesh->shape();
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) shared(data,tmp,shape)
        #endif
        for (int idx=0; idx < shape; ++idx)
            {
            data[idx] += tmp[idx];
            }
        }
    }

}
