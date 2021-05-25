#include "flyft/grand_potential.h"

#include <algorithm>

namespace flyft
{

GrandPotential::GrandPotential()
    {
    }

void GrandPotential::compute(std::shared_ptr<State> state)
    {
    allocate(state);
    auto mesh = state->getMesh();

    // evaluate functionals
    value_ = 0.0;
    if (ideal_)
        {
        ideal_->compute(state);
        value_ += ideal_->getValue();
        }
    if (excess_)
        {
        excess_->compute(state);
        value_ += excess_->getValue();
        }
    if (external_)
        {
        external_->compute(state);
        value_ += external_->getValue();
        }

    // sum up contributions to derivatives for each type
    for (const auto& t : state->getTypes())
        {
        auto d = derivatives_.at(t)->data();
        auto did = (ideal_) ? ideal_->getDerivative(t)->data() : nullptr;
        auto dex = (excess_) ? excess_->getDerivative(t)->data() : nullptr;
        auto dext = (external_) ? external_->getDerivative(t)->data() : nullptr;

        const auto shape = mesh->shape();
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) shared(d,did,dex,dext,shape)
        #endif
        for (int idx=0; idx < shape; ++idx)
            {
            double deriv = 0.;
            if (did) deriv += did[idx];
            if (dex) deriv += dex[idx];
            if (dext) deriv += dext[idx];
            d[idx] = deriv;
            }

        // subtract chemical potential part when component is open
        auto constraint_type = constraint_types_.at(t);
        if (constraint_type == Constraint::mu)
            {
            auto mu_bulk = constraints_.at(t);
            auto rho = state->getField(t)->data();
            const auto dx = mesh->step();
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) shared(shape,dx,mu_bulk,rho,d) reduction(-:value_)
            #endif
            for (int idx=0; idx < shape; ++idx)
                {
                d[idx] -= mu_bulk;
                value_ -= dx*mu_bulk*rho[idx];
                }
            }
        }
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
    ideal_ = ideal;
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
    excess_ = excess;
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
    external_ = external;
    }

const TypeMap<double>& GrandPotential::getConstraints()
    {
    return constraints_;
    }

double GrandPotential::getConstraint(const std::string& type) const
    {
    return constraints_.at(type);
    }

void GrandPotential::setConstraints(const TypeMap<double>& constraints)
    {
    constraints_ = TypeMap<double>(constraints);
    }

void GrandPotential::setConstraint(const std::string& type, double constraint)
    {
    constraints_[type] = constraint;
    }

const TypeMap<GrandPotential::Constraint>& GrandPotential::getConstraintTypes()
    {
    return constraint_types_;
    }

GrandPotential::Constraint GrandPotential::getConstraintType(const std::string& type) const
    {
    return constraint_types_.at(type);
    }

void GrandPotential::setConstraintTypes(const TypeMap<GrandPotential::Constraint>& constraint_types)
    {
    constraint_types_ = TypeMap<GrandPotential::Constraint>(constraint_types);
    }

void GrandPotential::setConstraintType(const std::string& type, GrandPotential::Constraint constraint_type)
    {
    constraint_types_[type] = constraint_type;
    }

}
