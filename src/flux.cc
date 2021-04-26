#include "flyft/flux.h"

#include <algorithm>

namespace flyft
{

Flux::Flux()
    {
    }

Flux::~Flux()
    {
    }

const TypeMap<std::shared_ptr<Field>>& Flux::getFluxes()
    {
    return fluxes_;
    }

std::shared_ptr<Field> Flux::getFlux(const std::string& type)
    {
    return fluxes_.at(type);
    }

std::shared_ptr<const Field> Flux::getFlux(const std::string& type) const
    {
    return fluxes_.at(type);
    }

void Flux::allocate(std::shared_ptr<State> state)
    {
    // purge stored types that are not in the state
    auto types = state->getTypes();
    for (auto it = fluxes_.cbegin(); it != fluxes_.cend(); /* no increment here */)
        {
        auto t = it->first;
        if (std::find(types.begin(), types.end(), t) == types.end())
            {
            it = fluxes_.erase(it);
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
        if (fluxes_.find(t) == fluxes_.end())
            {
            fluxes_[t] = std::make_shared<Field>(mesh->shape());
            }
        fluxes_[t]->reshape(mesh->shape());
        }
    }

bool Flux::validateConstraints(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state) const
    {
    bool valid = true;
    for (const auto& t : state->getTypes())
        {
        if (grand->getConstraintType(t) != GrandPotential::Constraint::N)
            {
            valid = false;
            break;
            }
        }
    return valid;
    }
}
