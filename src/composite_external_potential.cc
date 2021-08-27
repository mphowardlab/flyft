#include "flyft/composite_external_potential.h"

#include <algorithm>

namespace flyft
{

CompositeExternalPotential::CompositeExternalPotential()
    {
    }

bool CompositeExternalPotential::setup(std::shared_ptr<State> state, bool compute_value)
    {
    for (const auto& o : objects_)
        {
        o->compute(state,compute_value);
        }
    return ExternalPotential::setup(state,compute_value);
    }

void CompositeExternalPotential::computePotentials(std::shared_ptr<State> state)
    {
    // ensure objects are up-to-date, this will do nothing if nothing has changed
    for (const auto& o : objects_)
        {
        o->compute(state,false);
        }

    // fill total potential with zeros and accumulate
    for (const auto& t : state->getTypes())
        {
        auto d = derivatives_(t)->full_view();
        std::fill(d.begin(), d.end(), 0.);
        for (const auto& o : objects_)
            {
            auto df = o->getDerivative(t)->const_full_view();
            std::transform(d.begin(), d.end(), df.begin(), d.begin(), std::plus<>());
            }
        }
    }

void CompositeExternalPotential::requestDerivativeBuffer(const std::string& type, int buffer_request)
    {
    ExternalPotential::requestDerivativeBuffer(type,buffer_request);
    for (const auto& o : objects_)
        {
        o->requestDerivativeBuffer(type,buffer_request);
        }
    }

bool CompositeExternalPotential::addObject(std::shared_ptr<ExternalPotential> object)
    {
    bool added = CompositeMixin<ExternalPotential>::addObject(object);
    if (added)
        {
        compute_depends_.add(object.get());
        }
    return added;
    }

bool CompositeExternalPotential::removeObject(std::shared_ptr<ExternalPotential> object)
    {
    bool removed = CompositeMixin<ExternalPotential>::removeObject(object);
    if (removed)
        {
        compute_depends_.remove(object->id());
        }
    return removed;
    }

void CompositeExternalPotential::clearObjects()
    {
   if (objects_.size() > 0)
        {
        for (const auto& o : objects_)
            {
            compute_depends_.remove(o->id());
            }
        CompositeMixin<ExternalPotential>::clearObjects();
        }
    }

}
