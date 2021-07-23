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
        auto rho = state->getField(t)->begin();
        auto j = flux->getFlux(t)->cbegin();
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(timestep,mesh) shared(rho,j)
        #endif
        for (int idx=0; idx < mesh.shape(); ++idx)
            {
            const int self = mesh(idx);
            // change in density is flux in - flux out over time
            const auto rate = (j[self]-j[mesh(idx+1)])/mesh.step();
            rho[self] += timestep*rate;
            }
        }
    state->advanceTime(timestep);
    }

}
