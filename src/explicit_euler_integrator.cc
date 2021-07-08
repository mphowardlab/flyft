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
    // mesh properties
    const auto mesh = state->getMesh();
    const auto dx = mesh->step();
    const auto shape = mesh->shape();

    // evaluate fluxes and apply to volumes
    flux->compute(grand,state);
    for (const auto& t : state->getTypes())
        {
        auto rho = state->getField(t)->data();
        auto j = flux->getFlux(t)->data();
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(timestep,dx,shape) shared(rho,j)
        #endif
        for (int idx=0; idx < shape; ++idx)
            {
            // explicitly apply pbcs on the index
            // TODO: add a wrapping function to the mesh
            int left = idx;
            int right = (idx+1) % shape;

            // change in density is flux in - flux out over time
            const auto rate = (j[left]-j[right])/dx;
            rho[idx] += timestep*rate;
            }
        }
    state->advanceTime(timestep);
    }

}
