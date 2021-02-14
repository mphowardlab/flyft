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

const TypeMap<std::shared_ptr<Field>>& FreeEnergyFunctional::getDerivatives()
    {
    return derivatives_;
    }

std::shared_ptr<Field> FreeEnergyFunctional::getDerivative(const std::string& type)
    {
    return derivatives_[type];
    }

std::shared_ptr<const Field> FreeEnergyFunctional::getDerivative(const std::string& type) const
    {
    return derivatives_.at(type);
    }

void FreeEnergyFunctional::allocate(std::shared_ptr<State> state)
    {
    auto mesh = state->getMesh();

    // TODO: purge unused fields

    for (const auto& t : state->getTypes())
        {
        if (derivatives_.find(t) == derivatives_.end())
            {
            derivatives_[t] = std::make_shared<Field>(mesh->shape());
            }
        derivatives_[t]->reshape(mesh->shape());
        }
    }
}
