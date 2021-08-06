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
    return derivatives_.at(type);
    }

std::shared_ptr<const Field> Functional::getDerivative(const std::string& type) const
    {
    return derivatives_.at(type);
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
    // TODO: move this call to be explicit, programmers should know when they are intending a sync
    state->syncFields();

    // match derivatives to state types
    state->matchFields(derivatives_,buffer_requests_);
    }
}
