#include "flyft/piccard_iteration.h"

#include <cmath>

namespace flyft
{

PiccardIteration::PiccardIteration(std::shared_ptr<FreeEnergyFunctional> excess,
                                   std::shared_ptr<FreeEnergyFunctional> external,
                                   double mix_param,
                                   int max_iterations,
                                   double tolerance)
    : Solver(excess,external)
    {
    setMixParameter(mix_param);
    setMaxIterations(max_iterations);
    setTolerance(tolerance);
    }

bool PiccardIteration::solve(std::shared_ptr<State> state)
    {
    auto mesh = state->getMesh();
    auto alpha = getMixParameter();
    auto tol = getTolerance();

    // establish constraints on fields

    Field tmp(mesh->shape());
    bool converged = false;
    for (int iter=0; iter < getMaxIterations() && !converged; ++iter)
        {
        // propose converged, and invalidate if any change is too big
        converged = true;

        // compute current fields
        excess_->compute(state);
        external_->compute(state);

        // apply piccard mixing scheme
        for (int i=0; i < state->getNumFields(); ++i)
            {
            auto rho = state->getField(i)->data();
            auto rho_tmp = tmp.data();
            auto mu_ex = excess_->getDerivative(i)->data();
            auto V = external_->getDerivative(i)->data();

            double norm = 1.0;
            auto constraint_type = state->getConstraintType(i);
            if (constraint_type == State::Constraint::N)
                {
                auto N = state->getConstraint(i);
                double sum = 0.0;
                for (int idx=0; idx < mesh->shape(); ++idx)
                    {
                    rho_tmp[idx] = std::exp(-V[idx]-mu_ex[idx]);
                    sum += rho_tmp[idx];
                    }
                sum *= mesh->step();
                norm = N/sum;
                }
            else if (constraint_type == State::Constraint::mu)
                {
                auto mu_bulk = state->getConstraint(i);
                for (int idx=0; idx < mesh->shape(); ++idx)
                    {
                    rho_tmp[idx] = std::exp(-V[idx]-mu_ex[idx]+mu_bulk);
                    }
                norm = 1.0/state->getIdealVolume(i);
                }
            else
                {
                // don't know what to do
                }

            // apply Piccard mixing along with appropriate norm on value
            // during the same loop while checking convergence
            for (int idx=0; idx < mesh->shape(); ++idx)
                {
                const double drho = alpha*(norm*rho_tmp[idx]-rho[idx]);
                rho[idx] += drho;

                // check on convergence from absolute change in rho (might also want a percentage check)
                if (std::abs(drho) > tol) converged = false;
                }
            }
        }

    return converged;
    }

double PiccardIteration::getMixParameter() const
    {
    return mix_param_;
    }

void PiccardIteration::setMixParameter(double mix_param)
    {
    if (mix_param <= 0 || mix_param > 1.0)
        {
        // error: mix parameter must be between 0 and 1
        }
    mix_param_ = mix_param;
    }

int PiccardIteration::getMaxIterations() const
    {
    return max_iterations_;
    }

void PiccardIteration::setMaxIterations(int max_iterations)
    {
    if (max_iterations < 1)
        {
        // error: mix parameter must be at least 1
        }
    max_iterations_ = max_iterations;
    }

double PiccardIteration::getTolerance() const
    {
    return tolerance_;
    }

void PiccardIteration::setTolerance(double tolerance)
    {
    if (tolerance <= 0)
        {
        // error: tolerance must be positive
        }
    tolerance_ = tolerance;
    }

}
