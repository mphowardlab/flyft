#ifndef FLYFT_CRANK_NICOLSON_INTEGRATOR_H_
#define FLYFT_CRANK_NICOLSON_INTEGRATOR_H_

#include "flyft/grand_potential.h"
#include "flyft/integrator.h"
#include "flyft/iterative_algorithm_mixin.h"
#include "flyft/state.h"

#include <memory>

namespace flyft
{

class CrankNicolsonIntegrator : public Integrator, public IterativeAlgorithmMixin
    {
    public:
        CrankNicolsonIntegrator(double timestep,
                                double mix_param,
                                int max_iterations,
                                double tolerance);

        bool advance(std::shared_ptr<Flux> flux,
                     std::shared_ptr<GrandPotential> grand,
                     std::shared_ptr<State> state,
                     double time) override;

        double getTimestep() const;
        void setTimestep(double timestep);

        double getMixParameter() const;
        void setMixParameter(double mix_param);

    private:
        double timestep_;
        TypeMap<std::shared_ptr<Field>> current_fields_;
        TypeMap<std::shared_ptr<Field>> current_rates_;
        double mix_param_;
    };

}

#endif // FLYFT_CRANK_NICOLSON_INTEGRATOR_H_