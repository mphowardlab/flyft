#include "flyft/composite_functional.h"

#include <algorithm>

namespace flyft
{

CompositeFunctional::CompositeFunctional()
    {
    }

CompositeFunctional::CompositeFunctional(std::shared_ptr<FreeEnergyFunctional> functional)
    : CompositeFunctional(std::vector<std::shared_ptr<FreeEnergyFunctional>>({functional}))
    {
    }

CompositeFunctional::CompositeFunctional(const std::vector<std::shared_ptr<FreeEnergyFunctional>>& functionals)
    {
    for (const auto& f : functionals)
        {
        addFunctional(f);
        }
    }

void CompositeFunctional::compute(std::shared_ptr<State> state)
    {
    allocate(state);
    auto mesh = state->getMesh();

    // initialize to zeros
    value_ = 0.0;
    for (int i=0; i < state->getNumFields(); ++i)
        {
        auto d = derivatives_[i]->data();
        std::fill(d,d+mesh->shape(),0.);
        }

    // combine
    for (const auto& f : functionals_)
        {
        f->compute(state);

        // acumulate values
        const auto v = f->getValue();
        value_ += v;

        // accumulate derivatives
        for (int i=0; i < state->getNumFields(); ++i)
            {
            auto d = derivatives_[i]->data();
            auto df = f->getDerivative(i)->data();
            for (int idx=0; idx < mesh->shape(); ++idx)
                {
                d[idx] += df[idx];
                }
            }
        }
    }

void CompositeFunctional::addFunctional(std::shared_ptr<FreeEnergyFunctional> functional)
    {
    if(std::find(functionals_.begin(), functionals_.end(), functional) == functionals_.end())
        {
        functionals_.push_back(functional);
        }
    }

void CompositeFunctional::removeFunctional(std::shared_ptr<FreeEnergyFunctional> functional)
    {
    auto it = std::find(functionals_.begin(), functionals_.end(), functional);
    if (it != functionals_.end())
        {
        functionals_.erase(it);
        }
    }

}
