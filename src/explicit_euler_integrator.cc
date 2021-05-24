#include "flyft/explicit_euler_integrator.h"

#include <cmath>

namespace flyft
{

ExplicitEulerIntegrator::ExplicitEulerIntegrator(double timestep)
    {
    setTimestep(timestep);
    }

bool ExplicitEulerIntegrator::advance(std::shared_ptr<Flux> flux,
                                      std::shared_ptr<GrandPotential> grand,
                                      std::shared_ptr<State> state,
                                      double time)
    {
    // sign(time) = -1, 0, or +1
    const char time_sign = (time > 0) - (time < 0);
    double time_remain = std::abs(time);
    while (time_remain > 0)
        {
        const double dt = std::min(timestep_, time_remain);

        // evaluate fluxes and apply to volumes
        const auto mesh = state->getMesh();
        flux->compute(grand,state);

        for (const auto& t : state->getTypes())
            {
            auto rho = state->getField(t)->data();
            auto j = flux->getFlux(t)->data();

            const auto dx = mesh->step();
            const auto shape = mesh->shape();
            #pragma omp parallel for schedule(static) default(none) shared(time_sign,dt,rho,j,dx,shape)
            for (int idx=0; idx < shape; ++idx)
                {
                // explicitly apply pbcs on the index
                // TODO: add a wrapping function to the mesh
                int left = idx;
                int right = (idx+1) % shape;

                // change in density is flux in - flux out over time
                const auto rate = (j[left]-j[right])/dx;
                rho[idx] += (time_sign*dt)*rate;
                if (rho[idx] < 0)
                    {
                    // densities are strictly non-negative, clamp
                    // TODO: make this a tolerance so that huge negative numbers are an error
                    rho[idx] = 0.;
                    }
                }
            }

        state->advanceTime(time_sign*dt);
        time_remain -= dt;
        }

    return true;
    }

double ExplicitEulerIntegrator::getTimestep() const
    {
    return timestep_;
    }

void ExplicitEulerIntegrator::setTimestep(double timestep)
    {
    if (timestep <= 0)
        {
        // ERROR: timestep is positive
        }
    timestep_ = timestep;
    }

}
