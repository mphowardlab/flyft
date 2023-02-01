#include "flyft/picard_iteration.h"

#include <cmath>

namespace flyft
{

PicardIteration::PicardIteration(double mix_param,
                                 int max_iterations,
                                 double tolerance)
    : FixedPointAlgorithmMixin(mix_param,max_iterations,tolerance)
    {
    }

bool PicardIteration::solve(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state)
    {
    const auto mesh = state->getMesh()->local().get();
    const auto alpha = getMixParameter();
    const auto tol = getTolerance();

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
            excess->compute(state,false);
        if (external)
            external->compute(state,false);

        // apply picard mixing scheme
        for (const auto& t : state->getTypes())
            {
            auto rho = state->getField(t)->view();
            Field tmp(state->getField(t)->shape(),state->getField(t)->buffer_shape());
            auto rho_tmp = tmp.view();
            auto mu_ex = (excess) ? excess->getDerivative(t)->const_view() : Field::ConstantView();
            auto V = (external) ? external->getDerivative(t)->const_view() : Field::ConstantView();

            double norm = 1.0;
            auto constraint_type = grand->getConstraintTypes()(t);
            if (constraint_type == GrandPotential::Constraint::N)
                {
                auto N = grand->getConstraints()(t);
                double sum = 0.0;
                #ifdef FLYFT_OPENMP
                #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(mu_ex,V,rho_tmp) reduction(+:sum)
                #endif
                for (int idx=0; idx < mesh->shape(); ++idx)
                    {
                    double eff_energy = 0.0;
                    if (mu_ex)
                        {
                        eff_energy += mu_ex(idx);
                        }
                    if (V)
                        {
                        eff_energy += V(idx);
                        }
                    rho_tmp(idx) = std::exp(-eff_energy);
                    sum += mesh->integrateVolume(idx, rho(idx));
                    }
                sum = state->getCommunicator()->sum(sum);
                norm = N/sum;
                }
            else if (constraint_type == GrandPotential::Constraint::mu)
                {
                const auto mu_bulk = grand->getConstraints()(t);
                #ifdef FLYFT_OPENMP
                #pragma omp parallel for schedule(static) default(none) firstprivate(mesh,mu_bulk) shared(mu_ex,V,rho_tmp)
                #endif
                for (int idx=0; idx < mesh->shape(); ++idx)
                    {
                    double eff_energy = 0.0;
                    if (mu_ex)
                        {
                        eff_energy += mu_ex(idx);
                        }
                    if (V)
                        {
                        eff_energy += V(idx);
                        }
                    rho_tmp(idx) = std::exp(-eff_energy+mu_bulk);
                    }
                norm = 1.0/ideal->getVolumes()(t);
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
            for (int idx=0; idx < mesh->shape(); ++idx)
                {
                const double drho = alpha*(norm*rho_tmp(idx)-rho(idx));
                rho(idx) += drho;

                // check on convergence from absolute change in rho (might also want a percentage check)
                if (std::abs(drho) > tol)
                    {
                    // this will be a race in parallel code, but it doesn't matter because only one thread needs to hit false
                    converged = false;
                    }
                }
            }

        converged = state->getCommunicator()->all(converged);
        }

    return converged;
    }

}
