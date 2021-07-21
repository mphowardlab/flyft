#include "flyft/composite_external_potential.h"
#include "flyft/parallel.h"

namespace flyft
{

void CompositeExternalPotential::potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state)
    {
    // size temporary array
    const auto mesh = *state->getMesh();
    if (!Vtmp_)
        {
        Vtmp_ = std::make_shared<Field>(mesh.shape());
        }
    Vtmp_->reshape(mesh.shape());

    // fill total potential with zeros
    auto data = V->data();
    parallel::fill(data,mesh.shape(),0.);

    for (const auto& potential : objects_)
        {
        potential->potential(Vtmp_,type,state);
        auto tmp = Vtmp_->data();
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(data,tmp)
        #endif
        for (auto idx=mesh.first(); idx != mesh.last(); ++idx)
            {
            data[idx] += tmp[idx];
            }
        }
    }

}
