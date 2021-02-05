#include "flyft/piccard_iteration.h"

namespace flyft
{

PiccardIteration::PiccardIteration(std::shared_ptr<FreeEnergyFunctional> excess,
                                   std::shared_ptr<FreeEnergyFunctional> external,
                                   double mix_param,
                                   int max_iterations)
    : Solver(excess,external)
    {
    setMixParameter(mix_param);
    setMaxIterations(max_iterations);
    }

bool PiccardIteration::solve(std::shared_ptr<State> state)
    {
    auto mesh = state->getMesh();

    Field tmp(mesh->shape());
    for (int iter=0; iter < getMaxIterations(); ++iter)
        {
        excess_->compute(state);
        external_->compute(state);

        for (int i=0; i < state->getNumFields(); ++i)
            {
//             auto rho = state->getField(i)->data();
//             auto mu_ex = excess_->getDerivative(i)->data();
//             auto V = external_->getDerivative(i)->data();
//             for (int idx=0; idx < mesh->shape(); ++idx)
//                 {
//                 }
            }
        }

    return true;
    }

double PiccardIteration::getMixParameter() const
    {
    return mix_param_;
    }

void PiccardIteration::setMixParameter(double mix_param)
    {
    if (mix_param <= 0 || mix_param > 1.0)
        {
        // error: mix parameter must be between 0 and 1
        }
    mix_param_ = mix_param;
    }

int PiccardIteration::getMaxIterations() const
    {
    return max_iterations_;
    }

void PiccardIteration::setMaxIterations(int max_iterations)
    {
    if (max_iterations < 1)
        {
        // error: mix parameter must be at least 1
        }
    max_iterations_ = max_iterations;
    }

}
