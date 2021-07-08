#ifndef FLYFT_INTEGRATOR_H_
#define FLYFT_INTEGRATOR_H_

#include "flyft/flux.h"
#include "flyft/grand_potential.h"
#include "flyft/state.h"

#include <memory>

namespace flyft
{

class Integrator
    {
    public:
        Integrator(double timestep);
        virtual ~Integrator();

        // noncopyable / nonmovable
        Integrator(const Integrator&) = delete;
        Integrator(Integrator&&) = delete;
        Integrator& operator=(const Integrator&) = delete;
        Integrator& operator=(Integrator&&) = delete;

        virtual bool advance(std::shared_ptr<Flux> flux,
                             std::shared_ptr<GrandPotential> grand,
                             std::shared_ptr<State> state,
                             double time);

        double getTimestep() const;
        void setTimestep(double timestep);

    protected:
        double timestep_;

        virtual void step(std::shared_ptr<Flux> flux,
                          std::shared_ptr<GrandPotential> grand,
                          std::shared_ptr<State> state,
                          double timestep) = 0;
    };

}

#endif // FLYFT_INTEGRATOR_H_
