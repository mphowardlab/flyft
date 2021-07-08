#include "flyft/integrator.h"

namespace flyft
{

Integrator::Integrator(double timestep)
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
    // sign(time) = -1, 0, or +1
    const char time_sign = (time > 0) - (time < 0);
    double time_remain = std::abs(time);
    while (time_remain > 0)
        {
        const double dt = std::min(timestep_, time_remain);
        step(flux,grand,state,time_sign*dt);
        time_remain -= dt;
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

}
