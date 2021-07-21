#include "flyft/picard_iteration.h"

#include <cmath>

namespace flyft
{

PicardIteration::PicardIteration(double mix_param,
                                 int max_iterations,
                                 double tolerance)
    : IterativeAlgorithmMixin(max_iterations,tolerance)
    {
    setMixParameter(mix_param);
    }

bool PicardIteration::solve(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state)
    {
    const auto mesh = *state->getMesh();
    const auto alpha = getMixParameter();
    const auto tol = getTolerance();

    Field tmp(mesh.capacity());
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
                #ifdef FLYFT_OPENMP
                #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(mu_ex,V,rho_tmp) reduction(+:sum)
                #endif
                for (auto idx=mesh.first(); idx != mesh.last(); ++idx)
                    {
                    double eff_energy = 0.0;
                    if (mu_ex) eff_energy += mu_ex[idx];
                    if (V) eff_energy += V[idx];

                    rho_tmp[idx] = std::exp(-eff_energy);
                    sum += rho_tmp[idx];
                    }
                sum *= mesh.step();
                norm = N/sum;
                }
            else if (constraint_type == GrandPotential::Constraint::mu)
                {
                const auto mu_bulk = grand->getConstraint(t);
                #ifdef FLYFT_OPENMP
                #pragma omp parallel for schedule(static) default(none) firstprivate(mesh,mu_bulk) shared(mu_ex,V,rho_tmp)
                #endif
                for (auto idx=mesh.first(); idx != mesh.last(); ++idx)
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
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(mesh,alpha,norm,tol) shared(rho,rho_tmp,converged)
            #endif
            for (auto idx=mesh.first(); idx != mesh.last(); ++idx)
                {
                const double drho = alpha*(norm*rho_tmp[idx]-rho[idx]);
                rho[idx] += drho;

                // check on convergence from absolute change in rho (might also want a percentage check)
                if (std::abs(drho) > tol)
                    {
                    // this will be a race in parallel code, but it doesn't matter because only one thread needs to hit false
                    converged = false;
                    }
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

}
