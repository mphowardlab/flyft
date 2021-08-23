#include "flyft/explicit_euler_integrator.h"

#include <cmath>

namespace flyft
{

ExplicitEulerIntegrator::ExplicitEulerIntegrator(double timestep)
    : Integrator(timestep)
    {
    }

static void applyStep(int idx,
                      Field::View& rho,
                      const Field::ConstantView& j,
                      const Mesh& mesh,
                      double timestep)
    {
    const auto rate = (j(idx)-j(idx+1))/mesh.step();
    rho(idx) += timestep*rate;
    }

void ExplicitEulerIntegrator::step(std::shared_ptr<Flux> flux,
                                   std::shared_ptr<GrandPotential> grand,
                                   std::shared_ptr<State> state,
                                   double timestep)
    {
    // evaluate fluxes and apply to volumes
    const auto mesh = *state->getMesh()->local();
    flux->compute(grand,state);

    // start communication and apply update to interior points
    state->startSyncFields(flux->getFluxes());
    for (const auto& t : state->getTypes())
        {
        auto rho = state->getField(t)->view();
        auto j = flux->getFlux(t)->const_view();
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(timestep,mesh) shared(rho,j)
        #endif
        for (int idx=0; idx < mesh.shape()-1; ++idx)
            {
            applyStep(idx,rho,j,mesh,timestep);
            }
        }

    // finalize communication and apply update to right edge point
    state->endSyncFields(flux->getFluxes());
    for (const auto& t : state->getTypes())
        {
        auto rho = state->getField(t)->view();
        auto j = flux->getFlux(t)->const_view();
        applyStep(mesh.shape()-1,rho,j,mesh,timestep);;
        }

    state->advanceTime(timestep);
    }

}
