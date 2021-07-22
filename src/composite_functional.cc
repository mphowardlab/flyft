#include "flyft/composite_functional.h"

#include <algorithm>

namespace flyft
{

void CompositeFunctional::compute(std::shared_ptr<State> state)
    {
    allocate(state);

    // initialize to zeros
    value_ = 0.0;
    for (const auto& t : state->getTypes())
        {
        auto d = derivatives_.at(t);
        std::fill(d->first(),d->last(),0.);
        }

    // combine
    const auto mesh = *state->getMesh();
    for (const auto& f : objects_)
        {
        f->compute(state);

        // acumulate values
        const auto v = f->getValue();
        value_ += v;

        // accumulate derivatives
        for (const auto& t : state->getTypes())
            {
            auto d = derivatives_.at(t)->first();
            auto df = f->getDerivative(t)->first();
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(d,df)
            #endif
            for (int idx=0; idx < mesh.shape(); ++idx)
                {
                d[idx] += df[idx];
                }
            }
        }
    }

}
