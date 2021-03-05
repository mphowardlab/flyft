#include "flyft/free_energy_functional.h"

#include <algorithm>

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
    return derivatives_.at(type);
    }

std::shared_ptr<const Field> FreeEnergyFunctional::getDerivative(const std::string& type) const
    {
    return derivatives_.at(type);
    }

void FreeEnergyFunctional::allocate(std::shared_ptr<State> state)
    {
    // purge stored types that are not in the state
    auto types = state->getTypes();
    for (auto it = derivatives_.cbegin(); it != derivatives_.cend(); /* no increment here */)
        {
        auto t = it->first;
        if (std::find(types.begin(), types.end(), t) == types.end())
            {
            it = derivatives_.erase(it);
            }
        else
            {
            ++it;
            }
        }

    // ensure every type has a field with the right shape
    auto mesh = state->getMesh();
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
