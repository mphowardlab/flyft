#include "flyft/crank_nicolson_integrator.h"

#include <cmath>

namespace flyft
{

CrankNicolsonIntegrator::CrankNicolsonIntegrator(double timestep,
                                                 double mix_param,
                                                 int max_iterations,
                                                 double tolerance)
    : IterativeAlgorithmMixin(max_iterations,tolerance)
    {
    setTimestep(timestep);
    setMixParameter(mix_param);
    }

bool CrankNicolsonIntegrator::advance(std::shared_ptr<Flux> flux,
                                      std::shared_ptr<GrandPotential> grand,
                                      std::shared_ptr<State> state,
                                      double time)
    {
    state->syncFields(last_fields_);
    state->syncFields(last_rates_);

    // mesh properties
    const auto mesh = state->getMesh();
    const auto dx = mesh->step();
    const auto shape = mesh->shape();

    // sign(time) = -1, 0, or +1
    const char time_sign = (time > 0) - (time < 0);
    double time_remain = std::abs(time);
    while (time_remain > 0)
        {
        const double dt = std::min(timestep_, time_remain);

        // evaluate initial fluxes at the **current** timestep
        flux->compute(grand,state);
        for (const auto& t : state->getTypes())
            {
            auto rho = state->getField(t)->data();
            auto j = flux->getFlux(t)->data();
            auto last_rho = last_fields_.at(t)->data();
            auto last_rate = last_rates_.at(t)->data();

            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(time_sign,dt,dx,shape) \
                shared(rho,j,last_rho,last_rate)
            #endif
            for (int idx=0; idx < shape; ++idx)
                {
                // explicitly apply pbcs on the index
                // TODO: add a wrapping function to the mesh
                int left = idx;
                int right = (idx+1) % shape;

                // change in density is flux in - flux out over time
                last_rho[idx] = rho[idx];
                last_rate[idx] = (j[left]-j[right])/dx;
                }
            }

        // advance time of state to *next* point
        state->advanceTime(time_sign*dt);

        // solve nonlinear equation for **next** timestep by picard iteration
        const auto alpha = getMixParameter();
        const auto tol = getTolerance();
        bool converged = false;
        for (int iter=0; iter < getMaxIterations() && !converged; ++iter)
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
                #pragma omp parallel for schedule(static) default(none) firstprivate(time_sign,dt,dx,shape,alpha,tol) \
                    shared(next_rho,next_j,last_rho,last_rate,converged)
                #endif
                for (int idx=0; idx < shape; ++idx)
                    {
                    // explicitly apply pbcs on the index
                    // TODO: add a wrapping function to the mesh
                    int left = idx;
                    int right = (idx+1) % shape;
                    const double next_rate = (next_j[left]-next_j[right])/dx;
                    const double try_rho = last_rho[idx] + 0.5*(time_sign*dt)*(last_rate[idx]+next_rate);
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

        // decrease time counter now that update step is done
        time_remain -= dt;
        }

    return true;
    }

double CrankNicolsonIntegrator::getTimestep() const
    {
    return timestep_;
    }

void CrankNicolsonIntegrator::setTimestep(double timestep)
    {
    if (timestep <= 0)
        {
        // ERROR: timestep is positive
        }
    timestep_ = timestep;
    }

double CrankNicolsonIntegrator::getMixParameter() const
    {
    return mix_param_;
    }

void CrankNicolsonIntegrator::setMixParameter(double mix_param)
    {
    if (mix_param <= 0 || mix_param > 1.0)
        {
        // error: mix parameter must be between 0 and 1
        }
    mix_param_ = mix_param;
    }

}
