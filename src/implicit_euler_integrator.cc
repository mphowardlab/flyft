#include "flyft/implicit_euler_integrator.h"

#include <algorithm>
#include <cmath>

namespace flyft
{

ImplicitEulerIntegrator::ImplicitEulerIntegrator(double timestep,
                                                 double mix_param,
                                                 int max_iterations,
                                                 double tolerance)
    : Integrator(timestep), FixedPointAlgorithmMixin(mix_param,max_iterations,tolerance)
    {
    }

bool ImplicitEulerIntegrator::advance(std::shared_ptr<Flux> flux,
                                      std::shared_ptr<GrandPotential> grand,
                                      std::shared_ptr<State> state,
                                      double time)
    {
    state->matchFields(last_fields_);
    return Integrator::advance(flux,grand,state,time);
    }

void ImplicitEulerIntegrator::step(std::shared_ptr<Flux> flux,
                                   std::shared_ptr<GrandPotential> grand,
                                   std::shared_ptr<State> state,
                                   double timestep)
    {
    // copy densities at the **current** timestep
    for (const auto& t : state->getTypes())
        {
        auto f = state->getField(t)->const_view();
        std::copy(f.begin(),f.end(),last_fields_(t)->view().begin());
        }

    // advance time of state to *next* point
    state->advanceTime(timestep);

    // solve nonlinear equation for **next** timestep by fixed-point iteration
    const auto mesh = state->getMesh()->local().get();
    const auto alpha = getMixParameter();
    const auto tol = getTolerance();
    bool converged = false;
    for (int iter=0; iter < max_iterations_ && !converged; ++iter)
        {
        // propose converged, and invalidate if any change is too big
        converged = true;

        // get flux of the new state
        flux->compute(grand,state);

        // apply update
        for (const auto& t : state->getTypes())
            {
            auto last_rho = last_fields_(t)->const_view();
            auto next_rho = state->getField(t)->view();
            auto next_j = flux->getFlux(t)->const_view();

            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(timestep,mesh,alpha,tol) shared(next_rho,next_j,last_rho,converged)
            #endif
            for (int idx=0; idx < mesh->shape(); ++idx)
                {
                const double next_rate = mesh->integrateSurface(idx,next_j)/mesh->volume(idx);
                double try_rho = last_rho(idx) + timestep*next_rate;
                const double drho = alpha*(try_rho-next_rho(idx));
                next_rho(idx) += drho;
                if (drho > tol)
                    {
                    converged = false;
                    }
                }
            }

        converged = state->getCommunicator()->all(converged);
        }
    if (!converged)
        {
        // TODO: Decide how to handle failed convergence... warning, error?
        }
    }

}
