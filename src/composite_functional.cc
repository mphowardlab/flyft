#include "flyft/composite_functional.h"

#include <algorithm>

namespace flyft
{

CompositeFunctional::CompositeFunctional()
    {
    }

void CompositeFunctional::compute(std::shared_ptr<State> state, bool compute_value)
    {
    bool needs_compute = setup(state,compute_value);
    if (!needs_compute)
        {
        return;
        }

    // initialize to zeros
    value_ = 0.0;
    const auto mesh = *state->getMesh()->local();
    for (const auto& t : state->getTypes())
        {
        auto d = derivatives_(t)->view();
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
            auto d = derivatives_(t)->view();
            auto df = f->getDerivative(t)->const_view();
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(d,df)
            #endif
            for (int idx=0; idx < mesh.shape(); ++idx)
                {
                d(idx) += df(idx);
                }
            }
        }

    finalize(state,compute_value);
    }

void CompositeFunctional::requestDerivativeBuffer(const std::string& type, int buffer_request)
    {
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
        token_.stage();
        }
    return added;
    }

bool CompositeFunctional::removeObject(std::shared_ptr<Functional> object)
    {
    bool removed = CompositeMixin<Functional>::removeObject(object);
    if (removed)
        {
        compute_depends_.remove(object->id());
        token_.stage();
        }
    return removed;
    }

void CompositeFunctional::clearObjects()
    {
    if (objects_.size() > 0)
        {
        CompositeMixin<Functional>::clearObjects();
        token_.stage();
        }
    }

bool CompositeFunctional::setup(std::shared_ptr<State> state, bool compute_value)
    {
    bool compute = Functional::setup(state,compute_value);

    // compute on member objects and check if anything changed
    compute_depends_.capture();
    for (const auto& f : objects_)
        {
        f->compute(state,compute_value);
        }
    compute |= compute_depends_.changed();

    return compute;
    }

}
