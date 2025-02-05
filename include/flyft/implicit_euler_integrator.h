#ifndef FLYFT_IMPLICIT_EULER_INTEGRATOR_H_
#define FLYFT_IMPLICIT_EULER_INTEGRATOR_H_

#include "flyft/fixed_point_algorithm_mixin.h"
#include "flyft/grand_potential.h"
#include "flyft/integrator.h"
#include "flyft/state.h"

#include <memory>

namespace flyft
    {

class ImplicitEulerIntegrator : public Integrator, public FixedPointAlgorithmMixin
    {
    public:
    ImplicitEulerIntegrator(double timestep,
                            double mix_param,
                            int max_iterations,
                            double tolerance);

    bool advance(std::shared_ptr<Flux> flux,
                 std::shared_ptr<GrandPotential> grand,
                 std::shared_ptr<State> state,
                 double time) override;

    protected:
    void step(std::shared_ptr<Flux> flux,
              std::shared_ptr<GrandPotential> grand,
              std::shared_ptr<State> state,
              double timestep) override;

    int getLocalErrorExponent() const override
        {
        return 2;
        }

    private:
    TypeMap<std::shared_ptr<Field>> last_fields_;
    };

    } // namespace flyft

#endif // FLYFT_IMPLICIT_EULER_INTEGRATOR_H_
