#ifndef FLYFT_EXPLICIT_EULER_INTEGRATOR_H_
#define FLYFT_EXPLICIT_EULER_INTEGRATOR_H_

#include "flyft/grand_potential.h"
#include "flyft/integrator.h"
#include "flyft/state.h"

#include <memory>

namespace flyft
    {

class ExplicitEulerIntegrator : public Integrator
    {
    public:
    ExplicitEulerIntegrator(double timestep);

    protected:
    void step(std::shared_ptr<Flux> flux,
              std::shared_ptr<GrandPotential> grand,
              std::shared_ptr<State> state,
              double timestep) override;

    int getLocalErrorExponent() const override
        {
        return 2;
        }
    };

    } // namespace flyft

#endif // FLYFT_EXPLICIT_EULER_INTEGRATOR_H_
