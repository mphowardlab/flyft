#include "flyft/crank_nicolson_integrator.h"

#include <cmath>

namespace flyft
{

CrankNicolsonIntegrator::CrankNicolsonIntegrator(double timestep,
                                                 double mix_param,
                                                 int max_iterations,
                                                 double tolerance)
    : Integrator(timestep), FixedPointAlgorithmMixin(mix_param,max_iterations,tolerance)
    {
    }

bool CrankNicolsonIntegrator::advance(std::shared_ptr<Flux> flux,
                                      std::shared_ptr<GrandPotential> grand,
                                      std::shared_ptr<State> state,
                                      double time)
    {
    state->matchFields(last_fields_);
    state->matchFields(last_rates_);
    return Integrator::advance(flux,grand,state,time);
    }

void CrankNicolsonIntegrator::step(std::shared_ptr<Flux> flux,
                                   std::shared_ptr<GrandPotential> grand,
                                   std::shared_ptr<State> state,
                                   double timestep)
    {
    // evaluate initial fluxes at the **current** timestep
    const auto mesh = state->getMesh()->local().get();
    flux->compute(grand,state);
    for (const auto& t : state->getTypes())
        {
        auto rho = state->getField(t)->const_view();
        auto j = flux->getFlux(t)->const_view();
        auto last_rho = last_fields_(t)->view();
        auto last_rate = last_rates_(t)->view();

        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) \
            shared(rho,j,last_rho,last_rate)
        #endif
        for (int idx=0; idx < mesh->shape(); ++idx)
            {
            last_rho(idx) = rho(idx);
            last_rate(idx) = mesh->integrateSurface(idx,j(idx),j(idx+1))/mesh->volume(idx);
            }
        }

    // advance time of state to *next* point
    state->advanceTime(timestep);

    // solve nonlinear equation for **next** timestep by fixed-point iteration
    const auto alpha = mix_param_;
    const auto tol = tolerance_;
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
            auto last_rate = last_rates_(t)->const_view();
            auto next_rho = state->getField(t)->view();
            auto next_j = flux->getFlux(t)->const_view();

            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(timestep,mesh,alpha,tol) \
                shared(next_rho,next_j,last_rho,last_rate,converged)
            #endif
            for (int idx=0; idx < mesh->shape(); ++idx)
                {
                const double next_rate = (next_j(idx)-next_j(idx+1))/mesh->step();
                const double try_rho = last_rho(idx) + 0.5*timestep*(last_rate(idx)+next_rate);
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
