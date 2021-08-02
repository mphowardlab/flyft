#include "flyft/composite_functional.h"

#include <algorithm>

namespace flyft
{

void CompositeFunctional::compute(std::shared_ptr<State> state)
    {
    setup(state);

    // initialize to zeros
    value_ = 0.0;
    const auto mesh = *state->getMesh();
    for (const auto& t : state->getTypes())
        {
        auto d = derivatives_.at(t);
        std::fill(d->begin(),d->end(),0.);
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
            auto d = derivatives_.at(t)->begin();
            auto df = f->getDerivative(t)->cbegin();
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(d,df)
            #endif
            for (int idx=0; idx < mesh.shape(); ++idx)
                {
                d(idx) += df(idx);
                }
            }
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

}
