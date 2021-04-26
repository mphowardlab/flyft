#include "flyft/picard_iteration.h"

#include <cmath>

namespace flyft
{

PicardIteration::PicardIteration(double mix_param,
                                 int max_iterations,
                                 double tolerance)
    {
    setMixParameter(mix_param);
    setMaxIterations(max_iterations);
    setTolerance(tolerance);
    }

bool PicardIteration::solve(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state)
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
        auto ideal = grand->getIdealGasFunctional();
        auto excess = grand->getExcessFunctional();
        auto external = grand->getExternalPotential();
        if (excess)
            excess->compute(state);
        if (external)
            external->compute(state);

        // apply picard mixing scheme
        for (const auto& t : state->getTypes())
            {
            auto rho = state->getField(t)->data();
            auto rho_tmp = tmp.data();
            auto mu_ex = (excess) ? excess->getDerivative(t)->data() : nullptr;
            auto V = (external) ? external->getDerivative(t)->data() : nullptr;

            double norm = 1.0;
            auto constraint_type = grand->getConstraintType(t);
            if (constraint_type == GrandPotential::Constraint::N)
                {
                auto N = grand->getConstraint(t);
                double sum = 0.0;
                for (int idx=0; idx < mesh->shape(); ++idx)
                    {
                    double eff_energy = 0.0;
                    if (mu_ex) eff_energy += mu_ex[idx];
                    if (V) eff_energy += V[idx];

                    rho_tmp[idx] = std::exp(-eff_energy);
                    sum += rho_tmp[idx];
                    }
                sum *= mesh->step();
                norm = N/sum;
                }
            else if (constraint_type == GrandPotential::Constraint::mu)
                {
                auto mu_bulk = grand->getConstraint(t);
                for (int idx=0; idx < mesh->shape(); ++idx)
                    {
                    double eff_energy = 0.0;
                    if (mu_ex) eff_energy += mu_ex[idx];
                    if (V) eff_energy += V[idx];

                    rho_tmp[idx] = std::exp(-eff_energy+mu_bulk);
                    }
                norm = 1.0/ideal->getVolume(t);
                }
            else
                {
                // don't know what to do
                }

            // apply Picard mixing along with appropriate norm on value
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

double PicardIteration::getMixParameter() const
    {
    return mix_param_;
    }

void PicardIteration::setMixParameter(double mix_param)
    {
    if (mix_param <= 0 || mix_param > 1.0)
        {
        // error: mix parameter must be between 0 and 1
        }
    mix_param_ = mix_param;
    }

int PicardIteration::getMaxIterations() const
    {
    return max_iterations_;
    }

void PicardIteration::setMaxIterations(int max_iterations)
    {
    if (max_iterations < 1)
        {
        // error: mix parameter must be at least 1
        }
    max_iterations_ = max_iterations;
    }

double PicardIteration::getTolerance() const
    {
    return tolerance_;
    }

void PicardIteration::setTolerance(double tolerance)
    {
    if (tolerance <= 0)
        {
        // error: tolerance must be positive
        }
    tolerance_ = tolerance;
    }

}
