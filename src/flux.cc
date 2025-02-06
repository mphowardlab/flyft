#include "flyft/flux.h"

#include <algorithm>

namespace flyft
    {

Flux::Flux() {}

Flux::~Flux() {}

const TypeMap<std::shared_ptr<Field>>& Flux::getFluxes()
    {
    return fluxes_;
    }

std::shared_ptr<Field> Flux::getFlux(const std::string& type)
    {
    return fluxes_(type);
    }

std::shared_ptr<const Field> Flux::getFlux(const std::string& type) const
    {
    return fluxes_(type);
    }

int Flux::determineBufferShape(std::shared_ptr<State> /*state*/, const std::string& /*type*/)
    {
    return 0;
    }

void Flux::requestFluxBuffer(const std::string& type, int buffer_request)
    {
    auto it = buffer_requests_.find(type);
    if (it == buffer_requests_.end() || buffer_request > it->second)
        {
        buffer_requests_[type] = buffer_request;
        }
    }

void Flux::setup(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state)
    {
    if (!validateConstraints(grand, state))
        {
        // ERROR: need constant N in "grand" potential
        }

    // request derivative buffers (don't sync yet though)
    for (const auto& t : state->getTypes())
        {
        const auto buffer_request = determineBufferShape(state, t);
        state->requestFieldBuffer(t, buffer_request);
        grand->requestDerivativeBuffer(t, buffer_request);
        }

    // match up flux fields to state
    state->matchFields(fluxes_, buffer_requests_);
    }

bool Flux::validateConstraints(std::shared_ptr<GrandPotential> grand,
                               std::shared_ptr<State> state) const
    {
    bool valid = true;
    for (const auto& t : state->getTypes())
        {
        if (grand->getConstraintTypes()(t) != GrandPotential::Constraint::N)
            {
            valid = false;
            break;
            }
        }
    return valid;
    }
    } // namespace flyft
