#include "flyft/implicit_euler_integrator.h"
#include "flyft/parallel.h"

#include <cmath>

namespace flyft
{

ImplicitEulerIntegrator::ImplicitEulerIntegrator(double timestep,
                                                 double mix_param,
                                                 int max_iterations,
                                                 double tolerance)
    : IterativeAlgorithmMixin(max_iterations,tolerance)
    {
    setTimestep(timestep);
    setMixParameter(mix_param);
    }

bool ImplicitEulerIntegrator::advance(std::shared_ptr<Flux> flux,
                                      std::shared_ptr<GrandPotential> grand,
                                      std::shared_ptr<State> state,
                                      double time)
    {
    state->syncFields(last_fields_);

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

        // copy densities at the **current** timestep
        for (const auto& t : state->getTypes())
            {
            parallel::copy(state->getField(t)->data(),shape,last_fields_.at(t)->data());
            }

        // advance time of state to *next* point
        state->advanceTime(time_sign*dt);

        // solve nonlinear equation for **next** timestep by fixed-point iteration
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

                #ifdef FLYFT_OPENMP
                #pragma omp parallel for schedule(static) default(none) firstprivate(time_sign,dt,dx,shape,alpha,tol) shared(next_rho,next_j,last_rho,converged)
                #endif
                for (int idx=0; idx < shape; ++idx)
                    {
                    // explicitly apply pbcs on the index
                    // TODO: add a wrapping function to the mesh
                    int left = idx;
                    int right = (idx+1) % shape;
                    const double next_rate = (next_j[left]-next_j[right])/dx;
                    double try_rho = last_rho[idx] + (time_sign*dt)*next_rate;
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

double ImplicitEulerIntegrator::getTimestep() const
    {
    return timestep_;
    }

void ImplicitEulerIntegrator::setTimestep(double timestep)
    {
    if (timestep <= 0)
        {
        // ERROR: timestep is positive
        }
    timestep_ = timestep;
    }

double ImplicitEulerIntegrator::getMixParameter() const
    {
    return mix_param_;
    }

void ImplicitEulerIntegrator::setMixParameter(double mix_param)
    {
    if (mix_param <= 0 || mix_param > 1.0)
        {
        // error: mix parameter must be between 0 and 1
        }
    mix_param_ = mix_param;
    }

}
