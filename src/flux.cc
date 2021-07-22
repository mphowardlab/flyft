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
    state->syncFields(fluxes_);
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
