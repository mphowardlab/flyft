#include "flyft/composite_functional.h"

#include <algorithm>

namespace flyft
{

CompositeFunctional::CompositeFunctional()
    {
    }

bool CompositeFunctional::setup(std::shared_ptr<State> state, bool compute_value)
    {
    for (const auto& o : objects_)
        {
        o->compute(state,compute_value);
        }
    return Functional::setup(state,compute_value);
    }

void CompositeFunctional::_compute(std::shared_ptr<State> state, bool compute_value)
    {
    // initialize to zeros
    value_ = 0.0;
    for (const auto& t : state->getTypes())
        {
        auto d = derivatives_(t)->full_view();
        std::fill(d.begin(),d.end(),0.);
        }

    // combine
    for (const auto& f : objects_)
        {
        // acumulate values
        if (compute_value)
            {
            const auto v = f->getValue();
            value_ += v;
            }

        // accumulate derivatives
        for (const auto& t : state->getTypes())
            {
            auto d = derivatives_(t)->full_view();
            auto df = f->getDerivative(t)->const_full_view();
            std::transform(d.begin(), d.end(), df.begin(), d.begin(), std::plus<>());
            }
        }
    }

void CompositeFunctional::requestDerivativeBuffer(const std::string& type, int buffer_request)
    {
    Functional::requestDerivativeBuffer(type,buffer_request);
    for (const auto& f : objects_)
        {
        f->requestDerivativeBuffer(type,buffer_request);
        }
    }

int CompositeFunctional::determineBufferShape(std::shared_ptr<State> state, const std::string& type)
    {
    int max_buffer_shape = 0;
    for (const auto& f : objects_)
        {
        int buffer_shape = f->determineBufferShape(state,type);
        max_buffer_shape = std::max(buffer_shape,max_buffer_shape);
        }
    return max_buffer_shape;
    }

bool CompositeFunctional::addObject(std::shared_ptr<Functional> object)
    {
    bool added = CompositeMixin<Functional>::addObject(object);
    if (added)
        {
        compute_depends_.add(object.get());
        }
    return added;
    }

bool CompositeFunctional::removeObject(std::shared_ptr<Functional> object)
    {
    bool removed = CompositeMixin<Functional>::removeObject(object);
    if (removed)
        {
        compute_depends_.remove(object->id());
        }
    return removed;
    }

void CompositeFunctional::clearObjects()
    {
    if (objects_.size() > 0)
        {
        for (const auto& o : objects_)
            {
            compute_depends_.remove(o->id());
            }
        CompositeMixin<Functional>::clearObjects();
        }
    }

}
