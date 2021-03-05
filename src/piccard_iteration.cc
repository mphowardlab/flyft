#include "flyft/piccard_iteration.h"

#include <cmath>

namespace flyft
{

PiccardIteration::PiccardIteration(std::shared_ptr<GrandPotential> grand,
                                   double mix_param,
                                   int max_iterations,
                                   double tolerance)
    : Solver(grand)
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

    Field tmp(mesh->shape());
    bool converged = false;
    for (int iter=0; iter < getMaxIterations() && !converged; ++iter)
        {
        // propose converged, and invalidate if any change is too big
        converged = true;

        // compute current fields
        auto ideal = grand_->getIdealGasFunctional();
        auto excess = grand_->getExcessFunctional();
        auto external = grand_->getExternalPotential();
        excess->compute(state);
        external->compute(state);

        // apply piccard mixing scheme
        for (const auto& t : state->getTypes())
            {
            auto rho = state->getField(t)->data();
            auto rho_tmp = tmp.data();
            auto mu_ex = excess->getDerivative(t)->data();
            auto V = external->getDerivative(t)->data();

            double norm = 1.0;
            auto constraint_type = grand_->getConstraintType(t);
            if (constraint_type == GrandPotential::Constraint::N)
                {
                auto N = grand_->getConstraint(t);
                double sum = 0.0;
                for (int idx=0; idx < mesh->shape(); ++idx)
                    {
                    rho_tmp[idx] = std::exp(-V[idx]-mu_ex[idx]);
                    sum += rho_tmp[idx];
                    }
                sum *= mesh->step();
                norm = N/sum;
                }
            else if (constraint_type == GrandPotential::Constraint::mu)
                {
                auto mu_bulk = grand_->getConstraint(t);
                for (int idx=0; idx < mesh->shape(); ++idx)
                    {
                    rho_tmp[idx] = std::exp(-V[idx]-mu_ex[idx]+mu_bulk);
                    }
                norm = 1.0/ideal->getVolume(t);
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
