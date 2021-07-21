#ifndef FLYFT_PICCARD_ITERATION_H_
#define FLYFT_PICCARD_ITERATION_H_

#include "flyft/grand_potential.h"
#include "flyft/iterative_algorithm_mixin.h"
#include "flyft/state.h"
#include "flyft/solver.h"

#include <memory>

namespace flyft
{

class PicardIteration : public Solver, public IterativeAlgorithmMixin
    {
    public:
        PicardIteration(double mix_param,
                         int max_iterations,
                         double tolerance);

        bool solve(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state) override;

        double getMixParameter() const;
        void setMixParameter(double mix_param);

    private:
        double mix_param_;
    };

}

#endif // FLYFT_PICCARD_ITERATION_H_
