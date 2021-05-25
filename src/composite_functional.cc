#include "flyft/composite_functional.h"
#include "flyft/parallel.h"

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
        parallel::fill(d,mesh->shape(),0.);
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

            const auto shape = mesh->shape();
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) shared(d,df,shape)
            #endif
            for (int idx=0; idx < shape; ++idx)
                {
                d[idx] += df[idx];
                }
            }
        }
    }

}
