#include "flyft/explicit_euler_integrator.h"

#include <cmath>

namespace flyft
{

ExplicitEulerIntegrator::ExplicitEulerIntegrator(double timestep)
    : Integrator(timestep)
    {
    }

void ExplicitEulerIntegrator::step(std::shared_ptr<Flux> flux,
                                   std::shared_ptr<GrandPotential> grand,
                                   std::shared_ptr<State> state,
                                   double timestep)
    {
    // evaluate fluxes and apply to volumes
    const auto mesh = *state->getMesh();
    flux->compute(grand,state);
    for (const auto& t : state->getTypes())
        {
        auto rho = state->getField(t)->first();
        auto j = flux->getFlux(t)->first();
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(timestep,mesh) shared(rho,j)
        #endif
        for (int idx=0; idx < mesh.shape(); ++idx)
            {
            // explicitly apply pbcs on the index
            // TODO: remove this wrapping
            int left = idx;
            int right = (idx+1) % mesh.buffered_shape();

            // change in density is flux in - flux out over time
            const auto rate = (j[left]-j[right])/mesh.step();
            rho[idx] += timestep*rate;
            }
        }
    state->advanceTime(timestep);
    }

}
