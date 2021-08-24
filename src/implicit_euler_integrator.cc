#include "flyft/implicit_euler_integrator.h"

#include <algorithm>
#include <cmath>

namespace flyft
{

ImplicitEulerIntegrator::ImplicitEulerIntegrator(double timestep,
                                                 double mix_param,
                                                 int max_iterations,
                                                 double tolerance)
    : Integrator(timestep), FixedPointAlgorithmMixin(mix_param,max_iterations,tolerance)
    {
    }

bool ImplicitEulerIntegrator::advance(std::shared_ptr<Flux> flux,
                                      std::shared_ptr<GrandPotential> grand,
                                      std::shared_ptr<State> state,
                                      double time)
    {
    state->matchFields(last_fields_);
    return Integrator::advance(flux,grand,state,time);
    }

static bool applyStep(int idx,
                      Field::View& next_rho,
                      const Field::ConstantView& last_rho,
                      const Field::ConstantView& next_j,
                      const Mesh& mesh,
                      double tol,
                      double alpha,
                      double timestep)
    {
    const double next_rate = (next_j(idx)-next_j(idx+1))/mesh.step();
    double try_rho = last_rho(idx) + timestep*next_rate;
    const double drho = alpha*(try_rho-next_rho(idx));
    next_rho(idx) += drho;
    return (drho <= tol);
    }

void ImplicitEulerIntegrator::step(std::shared_ptr<Flux> flux,
                                   std::shared_ptr<GrandPotential> grand,
                                   std::shared_ptr<State> state,
                                   double timestep)
    {
    // copy densities at the **current** timestep
    for (const auto& t : state->getTypes())
        {
        auto f = state->getField(t)->const_view();
        std::copy(f.begin(),f.end(),last_fields_(t)->view().begin());
        }

    // advance time of state to *next* point
    state->advanceTime(timestep);

    // solve nonlinear equation for **next** timestep by fixed-point iteration
    const auto mesh = *state->getMesh()->local();
    const auto alpha = getMixParameter();
    const auto tol = getTolerance();
    bool converged = false;
    for (int iter=0; iter < max_iterations_ && !converged; ++iter)
        {
        // propose converged, and invalidate if any change is too big
        converged = true;

        // get flux of the new state
        flux->compute(grand,state);

        // start communication and apply update to interior points
        state->startSyncFields(flux->getFluxes());
        for (const auto& t : state->getTypes())
            {
            auto last_rho = last_fields_(t)->const_view();
            auto next_rho = state->getField(t)->view();
            auto next_j = flux->getFlux(t)->const_view();

            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(timestep,mesh,alpha,tol) shared(next_rho,next_j,last_rho,converged)
            #endif
            for (int idx=0; idx < mesh.shape()-1; ++idx)
                {
                bool conv = applyStep(idx,next_rho,last_rho,next_j,mesh,tol,alpha,timestep);
                if (!conv)
                    {
                    converged = false;
                    }
                }
            }

        // finalize communication and apply update to right edge point
        state->endSyncFields(flux->getFluxes());
        for (const auto& t : state->getTypes())
            {
            auto last_rho = last_fields_(t)->const_view();
            auto next_rho = state->getField(t)->view();
            auto next_j = flux->getFlux(t)->const_view();

            bool conv = applyStep(mesh.shape()-1,next_rho,last_rho,next_j,mesh,tol,alpha,timestep);
            if (!conv)
                {
                converged = false;
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
