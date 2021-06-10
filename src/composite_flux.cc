#include "flyft/composite_flux.h"
#include "flyft/parallel.h"

#include <algorithm>

namespace flyft
{

void CompositeFlux::compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state)
    {
    allocate(state);
    auto mesh = state->getMesh();

    // initialize to zeros
    for (const auto& t : state->getTypes())
        {
        auto j = fluxes_.at(t)->data();
        parallel::fill(j,mesh->shape(),0.);
        }

    // combine
    for (const auto& o : objects_)
        {
        o->compute(grand, state);
        for (const auto& t : state->getTypes())
            {
            auto j = fluxes_.at(t)->data();
            auto jo = o->getFlux(t)->data();

            const auto shape = mesh->shape();
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(shape) shared(j,jo)
            #endif
            for (int idx=0; idx < shape; ++idx)
                {
                j[idx] += jo[idx];
                }
            }
        }
    }

}
