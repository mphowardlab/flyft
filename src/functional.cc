#include "flyft/functional.h"

#include <algorithm>

namespace flyft
{

Functional::Functional()
    : value_(0.)
    {
    }

Functional::~Functional()
    {
    }

double Functional::getValue() const
    {
    return value_;
    }

const TypeMap<std::shared_ptr<Field>>& Functional::getDerivatives()
    {
    return derivatives_;
    }

std::shared_ptr<Field> Functional::getDerivative(const std::string& type)
    {
    return derivatives_(type);
    }

std::shared_ptr<const Field> Functional::getDerivative(const std::string& type) const
    {
    return derivatives_(type);
    }

void Functional::requestDerivativeBuffer(const std::string& type, int buffer_request)
    {
    auto it = buffer_requests_.find(type);
    if (it == buffer_requests_.end() || buffer_request > it->second)
        {
        buffer_requests_[type] = buffer_request;
        }
    }

int Functional::determineBufferShape(std::shared_ptr<State> /*state*/, const std::string& /*type*/)
    {
    return 0;
    }

const Functional::Token& Functional::token()
    {
    if (compute_depends_.changed())
        {
        compute_depends_.capture();
        token_.stage();
        }
    return TrackedObject::token();
    }

bool Functional::setup(std::shared_ptr<State> state)
    {
    // sync required fields
    for (const auto& t : state->getTypes())
        {
        state->requestFieldBuffer(t,determineBufferShape(state,t));
        }

    // match derivatives to state types
    state->matchFields(derivatives_,buffer_requests_);

    // return whether evaluation is required
    return (!compute_state_ || state->token() != compute_state_ || compute_depends_.changed());
    }

void Functional::finalize(std::shared_ptr<State> state)
    {
    // commit new values
    token_.stage();
    token_.commit();

    // capture dependencies
    compute_depends_.capture();
    compute_state_ = state->token();
    }

}
