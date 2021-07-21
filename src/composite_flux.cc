#include "flyft/composite_flux.h"
#include "flyft/parallel.h"

namespace flyft
{

void CompositeFlux::compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state)
    {
    allocate(state);
    const auto mesh = *state->getMesh();

    // initialize to zeros
    for (const auto& t : state->getTypes())
        {
        auto j = fluxes_.at(t)->data();
        parallel::fill(j,mesh.capacity(),0.);
        }

    // combine
    for (const auto& o : objects_)
        {
        o->compute(grand, state);
        for (const auto& t : state->getTypes())
            {
            auto j = fluxes_.at(t)->data();
            auto jo = o->getFlux(t)->data();
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(j,jo)
            #endif
            for (auto idx=mesh.first(); idx != mesh.last(); ++idx)
                {
                j[idx] += jo[idx];
                }
            }
        }
    }

}
