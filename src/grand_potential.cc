#include "flyft/grand_potential.h"

#include <algorithm>

namespace flyft
{

GrandPotential::GrandPotential()
    {
    compute_depends_.add(&constraints_);
    compute_depends_.add(&constraint_types_);
    }

bool GrandPotential::setup(std::shared_ptr<State> state, bool compute_value)
    {
    if (ideal_)
        {
        ideal_->compute(state,compute_value);
        }
    if (excess_)
        {
        excess_->compute(state,compute_value);
        }
    if (external_)
        {
        external_->compute(state,compute_value);
        }
    return Functional::setup(state,compute_value);
    }

void GrandPotential::_compute(std::shared_ptr<State> state, bool compute_value)
    {
    const auto mesh = state->getMesh()->local().get();

    // sum up contributions to derivatives for each type
    double constraint_value = 0.0;
    for (const auto& t : state->getTypes())
        {
        auto d = derivatives_(t)->full_view();
        auto did = (ideal_) ? ideal_->getDerivative(t)->const_full_view() : Field::ConstantView();
        auto dex = (excess_) ? excess_->getDerivative(t)->const_full_view() : Field::ConstantView();
        auto dext = (external_) ? external_->getDerivative(t)->const_full_view() : Field::ConstantView();
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(mesh) shared(d,did,dex,dext)
        #endif
        for (int idx=0; idx < d.size(); ++idx)
            {
            double deriv = 0.;
            if (did)
                {
                deriv += did(idx);
                }
            if (dex)
                {
                deriv += dex(idx);
                }
            if (dext)
                {
                deriv += dext(idx);
                }
            d(idx) = deriv;
            }

        // subtract chemical potential part when component is open
        auto constraint_type = constraint_types_(t);
        if (constraint_type == Constraint::mu)
            {
            const auto mu_bulk = constraints_(t);
            auto rho = state->getField(t)->const_view();
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(mesh,mu_bulk) \
            shared(rho,d,compute_value) reduction(-:constraint_value)
            #endif
            for (int idx=0; idx < mesh->shape(); ++idx)
                {
                d(idx) -= mu_bulk;
                if (compute_value)
                    {
                    constraint_value -= mu_bulk*mesh->integrateVolume(idx,rho(idx));
                    }
                }
            }
        }

    // reduce and add constraint contribution to value
    if (compute_value)
        {
        value_ = 0.0;
        if (ideal_)
            {
            value_ += ideal_->getValue();
            }
        if (excess_)
            {
            value_ += excess_->getValue();
            }
        if (external_)
            {
            value_ += external_->getValue();
            }
        value_ += state->getCommunicator()->sum(constraint_value);
        }
    }

void GrandPotential::requestDerivativeBuffer(const std::string& type, int buffer_request)
    {
    Functional::requestDerivativeBuffer(type,buffer_request);
    if (ideal_)
        {
        ideal_->requestDerivativeBuffer(type,buffer_request);
        }
    if (excess_)
        {
        excess_->requestDerivativeBuffer(type,buffer_request);
        }
    if (external_)
        {
        external_->requestDerivativeBuffer(type,buffer_request);
        }
    }

int GrandPotential::determineBufferShape(std::shared_ptr<State> state, const std::string& type)
    {
    int max_buffer_shape = 0;
    if (ideal_)
        {
        max_buffer_shape = std::max(ideal_->determineBufferShape(state,type),max_buffer_shape);
        }
    if (excess_)
        {
        max_buffer_shape = std::max(excess_->determineBufferShape(state,type),max_buffer_shape);
        }
    if (external_)
        {
        max_buffer_shape = std::max(external_->determineBufferShape(state,type),max_buffer_shape);
        }
    return max_buffer_shape;
    }

std::shared_ptr<IdealGasFunctional> GrandPotential::getIdealGasFunctional()
    {
    return ideal_;
    }

std::shared_ptr<const IdealGasFunctional> GrandPotential::getIdealGasFunctional() const
    {
    return ideal_;
    }

void GrandPotential::setIdealGasFunctional(std::shared_ptr<IdealGasFunctional> ideal)
    {
    if (ideal_)
        {
        compute_depends_.remove(ideal_->id());
        }
    ideal_ = ideal;
    if (ideal_)
        {
        compute_depends_.add(ideal_.get());
        }
    }

std::shared_ptr<Functional> GrandPotential::getExcessFunctional()
    {
    return excess_;
    }

std::shared_ptr<const Functional> GrandPotential::getExcessFunctional() const
    {
    return excess_;
    }

void GrandPotential::setExcessFunctional(std::shared_ptr<Functional> excess)
    {
    if (excess_)
        {
        compute_depends_.remove(excess_->id());
        }
    excess_ = excess;
    if (excess_)
        {
        compute_depends_.add(excess_.get());
        }
    }

std::shared_ptr<ExternalPotential> GrandPotential::getExternalPotential()
    {
    return external_;
    }

std::shared_ptr<const ExternalPotential> GrandPotential::getExternalPotential() const
    {
    return external_;
    }

void GrandPotential::setExternalPotential(std::shared_ptr<ExternalPotential> external)
    {
    if (external_)
        {
        compute_depends_.remove(external_->id());
        }
    external_ = external;
    if (external_)
        {
        compute_depends_.add(external_.get());
        }
    }

TypeMap<double>& GrandPotential::getConstraints()
    {
    return constraints_;
    }

const TypeMap<double>& GrandPotential::getConstraints() const
    {
    return constraints_;
    }

TypeMap<GrandPotential::Constraint>& GrandPotential::getConstraintTypes()
    {
    return constraint_types_;
    }

const TypeMap<GrandPotential::Constraint>& GrandPotential::getConstraintTypes() const
    {
    return constraint_types_;
    }

}
