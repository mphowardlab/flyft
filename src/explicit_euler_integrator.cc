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
    const auto mesh = state->getMesh()->local().get();
    flux->compute(grand,state);
    for (const auto& t : state->getTypes())
        {
        auto rho = state->getField(t)->view();
        auto j = flux->getFlux(t)->const_view();
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(timestep,mesh) shared(rho,j)
        #endif
        for (int idx=0; idx < mesh->shape(); ++idx)
            {
            const auto rate = mesh->integrateSurface(idx,j(idx),j(idx+1))/mesh->volume(idx);
            rho(idx) += timestep*rate;
            }
        }

    state->advanceTime(timestep);
    }

}
