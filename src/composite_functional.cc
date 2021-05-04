#include "flyft/composite_functional.h"

#include <algorithm>

namespace flyft
{

void CompositeFunctional::compute(std::shared_ptr<State> state)
    {
    allocate(state);
    auto mesh = state->getMesh();

    // initialize to zeros
    value_ = 0.0;
    for (const auto& t : state->getTypes())
        {
        auto d = derivatives_.at(t)->data();
        std::fill(d,d+mesh->shape(),0.);
        }

    // combine
    for (const auto& f : objects_)
        {
        f->compute(state);

        // acumulate values
        const auto v = f->getValue();
        value_ += v;

        // accumulate derivatives
        for (const auto& t : state->getTypes())
            {
            auto d = derivatives_.at(t)->data();
            auto df = f->getDerivative(t)->data();
            for (int idx=0; idx < mesh->shape(); ++idx)
                {
                d[idx] += df[idx];
                }
            }
        }
    }

}
