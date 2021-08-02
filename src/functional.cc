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

int Functional::determineBufferShape(std::shared_ptr<State> /*state*/, const std::string& /*type*/)
    {
    return 0;
    }

void Functional::setup(std::shared_ptr<State> state)
    {
    auto comm = state->getCommunicator();
    for (const auto& t : state->getTypes())
        {
        auto f = state->getField(t);
        f->requestBuffer(determineBufferShape(state,t));
        comm->sync(f);
        }
    state->syncFields(derivatives_);
    }
}
