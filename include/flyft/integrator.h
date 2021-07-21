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
        explicit Integrator(double timestep);
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

        bool usingAdaptiveTimestep() const;
        void enableAdaptiveTimestep(bool enable);

        double getAdaptiveTimestepDelay() const;
        void setAdaptiveTimestepDelay(double delay);

        double getAdaptiveTimestepTolerance() const;
        void setAdaptiveTimestepTolerance(double tolerance);

        double getAdaptiveTimestepMinimum() const;
        void setAdaptiveTimestepMinimum(double timestep);

    protected:
        double timestep_;

        bool use_adaptive_timestep_;
        double adaptive_timestep_delay_;
        double adaptive_timestep_tol_;
        double adaptive_timestep_min_;
        std::shared_ptr<State> adaptive_cur_state_;
        std::shared_ptr<State> adaptive_err_state_;

        virtual void step(std::shared_ptr<Flux> flux,
                          std::shared_ptr<GrandPotential> grand,
                          std::shared_ptr<State> state,
                          double timestep) = 0;

        virtual int getLocalErrorExponent() const = 0;
    };

}

#endif // FLYFT_INTEGRATOR_H_
