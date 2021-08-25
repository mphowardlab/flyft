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

void CompositeExternalPotential::potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state)
    {
    // ensure objects are up-to-date, this will do nothing if nothing has changed
    for (const auto& o : objects_)
        {
        o->compute(state,false);
        }

    // fill total potential with zeros and accumulate
    const auto mesh = *state->getMesh()->local();
    auto data = V->view();
    std::fill(data.begin(),data.end(),0.);
    for (const auto& o : objects_)
        {
        auto o_data = o->getDerivative(type)->const_view();
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(data,o_data)
        #endif
        for (int idx=0; idx < mesh.shape(); ++idx)
            {
            data(idx) += o_data(idx);
            }
        }
    }

bool CompositeExternalPotential::addObject(std::shared_ptr<ExternalPotential> object)
    {
    bool added = CompositeMixin<ExternalPotential>::addObject(object);
    if (added)
        {
        compute_depends_.add(object.get());
        token_.stage();
        }
    return added;
    }

bool CompositeExternalPotential::removeObject(std::shared_ptr<ExternalPotential> object)
    {
    bool removed = CompositeMixin<ExternalPotential>::removeObject(object);
    if (removed)
        {
        compute_depends_.remove(object->id());
        token_.stage();
        }
    return removed;
    }

void CompositeExternalPotential::clearObjects()
    {
   if (objects_.size() > 0)
        {
        CompositeMixin<ExternalPotential>::clearObjects();
        token_.stage();
        }
    }

}
