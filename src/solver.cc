#include "flyft/solver.h"

namespace flyft
{

Solver::Solver()
    {
    }

Solver::Solver(std::shared_ptr<FreeEnergyFunctional> excess)
    : excess_(excess)
    {
    }

Solver::Solver(std::shared_ptr<FreeEnergyFunctional> excess,
               std::shared_ptr<FreeEnergyFunctional> external)
    : excess_(excess), external_(external)
    {
    }

Solver::~Solver()
    {
    }

std::shared_ptr<FreeEnergyFunctional> Solver::getExcessFunctional()
    {
    return excess_;
    }

std::shared_ptr<const FreeEnergyFunctional> Solver::getExcessFunctional() const
    {
    return excess_;
    }

void Solver::setExcessFunctional(std::shared_ptr<FreeEnergyFunctional> excess)
    {
    excess_ = excess;
    }

std::shared_ptr<FreeEnergyFunctional> Solver::getExternalFunctional()
    {
    return external_;
    }

std::shared_ptr<const FreeEnergyFunctional> Solver::getExternalFunctional() const
    {
    return external_;
    }

void Solver::setExternalFunctional(std::shared_ptr<FreeEnergyFunctional> external)
    {
    external_ = external;
    }
}
