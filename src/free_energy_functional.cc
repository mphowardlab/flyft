#include "flyft/free_energy_functional.h"

namespace flyft
{

FreeEnergyFunctional::FreeEnergyFunctional()
    : value_(0.)
    {
    }

FreeEnergyFunctional::~FreeEnergyFunctional()
    {
    }

double FreeEnergyFunctional::getValue() const
    {
    return value_;
    }

const std::vector<std::shared_ptr<Field>>& FreeEnergyFunctional::getDerivatives()
    {
    return derivatives_;
    }

std::shared_ptr<Field> FreeEnergyFunctional::getDerivative(int idx)
    {
    return derivatives_[idx];
    }

std::shared_ptr<const Field> FreeEnergyFunctional::getDerivative(int idx) const
    {
    return std::static_pointer_cast<const Field>(derivatives_[idx]);
    }

void FreeEnergyFunctional::allocate(std::shared_ptr<State> state)
    {
    derivatives_.resize(state->getNumFields());
    for (auto& d : derivatives_)
        {
        if (!d)
            {
            d = std::make_shared<Field>(state->getMesh());
            }
        else
            {
            d->setMesh(state->getMesh());
            }
        }
    }

}
