#include "flyft/functional.h"

#include <algorithm>
#include <iostream>

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

bool Functional::setup(std::shared_ptr<State> state, bool compute_value)
    {
    // sync required fields
    for (const auto& t : state->getTypes())
        {
        state->requestFieldBuffer(t,determineBufferShape(state,t));
        }

    // match derivatives to state types
    state->matchFields(derivatives_,buffer_requests_);
    
    // return whether evaluation is required
    bool compute = (!compute_token_ ||
                    !compute_state_token_ ||
                    token() != compute_token_ ||
                    state->token() != compute_state_token_ ||
                    (compute_value && std::isnan(value_)));

    return compute;
    }

void Functional::finalize(std::shared_ptr<State> state, bool compute_value)
    {
    // make sure value has NaN if not computed
    if (!compute_value)
        {
        value_ = std::nan("");
        }

    // stage changes after a compute
    token_.stage();

    // capture dependencies
    compute_token_ = token();
    compute_state_token_ = state->token();
    }

}
