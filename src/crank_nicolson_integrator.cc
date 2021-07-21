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
    state->syncFields(last_fields_);
    state->syncFields(last_rates_);
    return Integrator::advance(flux,grand,state,time);
    }

void CrankNicolsonIntegrator::step(std::shared_ptr<Flux> flux,
                                   std::shared_ptr<GrandPotential> grand,
                                   std::shared_ptr<State> state,
                                   double timestep)
    {
    // evaluate initial fluxes at the **current** timestep
    const auto mesh = *state->getMesh();
    flux->compute(grand,state);
    for (const auto& t : state->getTypes())
        {
        auto rho = state->getField(t)->data();
        auto j = flux->getFlux(t)->data();
        auto last_rho = last_fields_.at(t)->data();
        auto last_rate = last_rates_.at(t)->data();

        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) \
            shared(rho,j,last_rho,last_rate)
        #endif
        for (auto idx=mesh.first(); idx != mesh.last(); ++idx)
            {
            // explicitly apply pbcs on the index
            // TODO: remove this wrapping
            int left = idx;
            int right = (idx+1) % mesh.shape();

            // change in density is flux in - flux out over time
            last_rho[idx] = rho[idx];
            last_rate[idx] = (j[left]-j[right])/mesh.step();
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

        // check for convergence new state
        for (const auto& t : state->getTypes())
            {
            auto next_rho = state->getField(t)->data();
            auto next_j = flux->getFlux(t)->data();
            auto last_rho = last_fields_.at(t)->data();
            auto last_rate = last_rates_.at(t)->data();

            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(timestep,mesh,alpha,tol) \
                shared(next_rho,next_j,last_rho,last_rate,converged)
            #endif
            for (auto idx=mesh.first(); idx != mesh.last(); ++idx)
                {
                // explicitly apply pbcs on the index
                // TODO: remove this wrapping
                int left = idx;
                int right = (idx+1) % mesh.shape();
                const double next_rate = (next_j[left]-next_j[right])/mesh.step();
                const double try_rho = last_rho[idx] + 0.5*timestep*(last_rate[idx]+next_rate);
                const double drho = alpha*(try_rho-next_rho[idx]);
                if (drho > tol)
                    {
                    converged = false;
                    }
                next_rho[idx] += drho;
                }
            }
        }
    if (!converged)
        {
        // TODO: Decide how to handle failed convergence... warning, error?
        }
    }

}
