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

void Functional::setup(std::shared_ptr<State> state)
    {
    // sync required fields
    for (const auto& t : state->getTypes())
        {
        state->requestFieldBuffer(t,determineBufferShape(state,t));
        }

    // match derivatives to state types
    state->matchFields(derivatives_,buffer_requests_);
    }

bool Functional::needsCompute(std::shared_ptr<State> state)
    {
    return (state->token() != state_token_ || depends_.changed());
    }

void Functional::finalize(std::shared_ptr<State> state)
    {
    // commit new values
    token_.stage();
    token_.commit();

    // capture dependencies
    depends_.capture();
    state_token_ = state->token();
    }

}
