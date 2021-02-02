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
    return derivatives_[idx];
    }

void FreeEnergyFunctional::allocate(std::shared_ptr<State> state)
    {
    derivatives_.resize(state->getNumFields());

    auto mesh = state->getMesh();
    for (auto& d : derivatives_)
        {
        if (!d)
            {
            d = std::make_shared<Field>(mesh->shape());
            }
        d->reshape(mesh->shape());
        }
    }

}
