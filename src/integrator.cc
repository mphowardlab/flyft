#include "flyft/integrator.h"
#include <stdexcept>

namespace flyft
{

Integrator::Integrator(double timestep)
    : use_adaptive_timestep_(false), adaptive_timestep_delay_(0),
      adaptive_timestep_tol_(1e-8), adaptive_timestep_min_(1e-8)
    {
    setTimestep(timestep);
    }

Integrator::~Integrator()
    {
    }

bool Integrator::advance(std::shared_ptr<Flux> flux,
                         std::shared_ptr<GrandPotential> grand,
                         std::shared_ptr<State> state,
                         double time)
    {
    // alloc adaptive step state memory
    if (use_adaptive_timestep_)
        {
        if (!adaptive_cur_state_)
            {
            adaptive_cur_state_ = std::make_shared<State>(state->getMesh(),state->getTypes());
            }
        if (!adaptive_err_state_)
            {
            adaptive_err_state_ = std::make_shared<State>(state->getMesh(),state->getTypes());
            }
        }
    else
        {
        adaptive_cur_state_ = nullptr;
        adaptive_err_state_ = nullptr;
        }

    // sign(time) = -1, 0, or +1
    const char time_sign = (time > 0) - (time < 0);
    double time_remain = std::abs(time);
    double adaptive_last_remain = time_remain;
    while (time_remain > 0)
        {
        if (use_adaptive_timestep_ &&
            std::abs(adaptive_last_remain-time_remain) >= adaptive_timestep_delay_ &&
            2.*timestep_ < time_remain)
            {
            adaptive_last_remain = time_remain;
            const auto mesh = *state->getMesh();

            *adaptive_cur_state_ = *state;

            double dt_try = timestep_;
            double dt_next = timestep_;
            bool converged = false;
            while (!converged)
                {
                if (dt_try < adaptive_timestep_min_)
                    {
                    throw std::runtime_error("timestep decreased too much");
                    }

                // take two normal steps
                step(flux,grand,state,time_sign*dt_try);
                step(flux,grand,state,time_sign*dt_try);

                // then take double-sized step
                *adaptive_err_state_ = *adaptive_cur_state_;
                step(flux,grand,adaptive_err_state_,2*time_sign*dt_try);

                // compute error between the two
                double max_err = 0.;
                for (const auto& t : state->getTypes())
                    {
                    auto rho = state->getField(t)->cbegin();
                    auto rho_err = adaptive_err_state_->getField(t)->cbegin();
                    // find max error on mesh
                    double type_max_err = 0.;
                    #ifdef FLYFT_OPENMP
                    #pragma omp parallel for schedule(static) default(none) firstprivate(mesh,rho,rho_err) \
                        reduction(max:type_max_err)
                    #endif
                    for (int idx=0; idx < mesh.shape(); ++idx)
                        {
                        const int self = mesh(idx);
                        const double err = std::abs(rho_err[self]-rho[self]);
                        if (err > type_max_err)
                            {
                            type_max_err = err;
                            }
                        }
                    // pool max by type
                    if (type_max_err > max_err)
                        {
                        max_err = type_max_err;
                        }
                    }

                // if error is greater than tolerance, reject the step and try again
                // otherwise, accept the step and scale up if possible
                if (max_err > adaptive_timestep_tol_)
                    {
                    converged = false;
                    dt_try *= std::max(0.9*std::pow(adaptive_timestep_tol_/max_err,1./getLocalErrorExponent()),0.1);
                    *state = *adaptive_cur_state_;
                    }
                else
                    {
                    converged = true;
                    // success, set next timestep and guard against divide by zero
                    if (max_err > 0)
                        {
                        dt_next = dt_try*std::min(0.9*std::pow(adaptive_timestep_tol_/max_err,1./getLocalErrorExponent()),5.);
                        }
                    else
                        {
                        dt_next = 5.*dt_try;
                        }
                    }
                }

            if (!converged)
                {
                throw std::runtime_error("failed to converge");
                }
            time_remain -= 2*dt_try;
            timestep_ = dt_next;
            }
        else
            {
            const double dt = std::min(timestep_,time_remain);
            step(flux,grand,state,time_sign*dt);
            time_remain -= dt;
            }
        }
    return true;
    }

double Integrator::getTimestep() const
    {
    return timestep_;
    }

void Integrator::setTimestep(double timestep)
    {
    if (timestep <= 0)
        {
        // ERROR: timestep is positive
        }
    timestep_ = timestep;
    }

double Integrator::getAdaptiveTimestepDelay() const
    {
    return adaptive_timestep_delay_;
    }

void Integrator::setAdaptiveTimestepDelay(double delay)
    {
    if (delay < 0)
        {
        // ERROR: timestep delay must be positive
        }
    adaptive_timestep_delay_ = delay;
    }

double Integrator::getAdaptiveTimestepTolerance() const
    {
    return adaptive_timestep_tol_;
    }

void Integrator::setAdaptiveTimestepTolerance(double tolerance)
    {
    if (tolerance <= 0.)
        {
        // ERROR: tolerance must be positive
        }
    adaptive_timestep_tol_ = tolerance;
    }

double Integrator::getAdaptiveTimestepMinimum() const
    {
    return adaptive_timestep_min_;
    }

void Integrator::setAdaptiveTimestepMinimum(double timestep)
    {
    if (timestep < 0)
        {
        // ERROR: minimum timestep must be nonnegative
        }
    adaptive_timestep_min_ = timestep;
    }

bool Integrator::usingAdaptiveTimestep() const
    {
    return use_adaptive_timestep_;
    }

void Integrator::enableAdaptiveTimestep(bool enable)
    {
    use_adaptive_timestep_ = enable;
    }

}
