#include "flyft/grand_potential.h"

#include <algorithm>

namespace flyft
{

GrandPotential::GrandPotential()
    {
    ideal_ = std::make_shared<IdealGasFunctional>();
    intrinsic_ = std::make_shared<CompositeFunctional>(ideal_);
    excess_ = std::make_shared<CompositeFunctional>();
    external_ = std::make_shared<CompositeFunctional>();
    }

void GrandPotential::compute(std::shared_ptr<State> state)
    {
    allocate(state);
    auto mesh = state->getMesh();

    // evaluate functionals
    intrinsic_->compute(state);
    external_->compute(state);

    // sum up contributions
    value_ = intrinsic_->getValue() + external_->getValue();
    for (const auto& t : state->getTypes())
        {
        auto d = derivatives_[t]->data();
        auto dint = intrinsic_->getDerivative(t)->data();
        auto dext = external_->getDerivative(t)->data();
        for (int idx=0; idx < mesh->shape(); ++idx)
            {
            d[idx] = dint[idx] + dext[idx];
            }

        // subtract chemical potential part when component is open
        auto constraint_type = constraint_types_.at(t);
        if (constraint_type == Constraint::mu)
            {
            auto mu_bulk = constraints_.at(t);
            auto rho = state->getField(t)->data();
            for (int idx=0; idx < mesh->shape(); ++idx)
                {
                value_ -= mesh->step()*mu_bulk*rho[idx];
                d[idx] -= mu_bulk;
                }
            }
        }
    }

std::shared_ptr<IdealGasFunctional> GrandPotential::getIdealGasFunctional()
    {
    return ideal_;
    }

std::shared_ptr<CompositeFunctional> GrandPotential::getIntrinsicFunctional()
    {
    return intrinsic_;
    }

std::shared_ptr<CompositeFunctional> GrandPotential::getExcessIntrinsicFunctional()
    {
    return excess_;
    }

std::shared_ptr<CompositeFunctional> GrandPotential::getExternalPotential()
    {
    return external_;
    }

void GrandPotential::addExcessFunctional(std::shared_ptr<FreeEnergyFunctional> functional)
    {
    intrinsic_->addFunctional(functional);
    excess_->addFunctional(functional);
    }

void GrandPotential::removeExcessFunctional(std::shared_ptr<FreeEnergyFunctional> functional)
    {
    intrinsic_->removeFunctional(functional);
    excess_->removeFunctional(functional);
    }

void GrandPotential::addExternalPotential(std::shared_ptr<ExternalPotential> functional)
    {
    external_->addFunctional(functional);
    }

void GrandPotential::removeExternalPotential(std::shared_ptr<ExternalPotential> functional)
    {
    external_->removeFunctional(functional);
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

void GrandPotential::setConstraint(const std::string& type, double value)
    {
    constraints_[type] = value;
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

void GrandPotential::setConstraintType(const std::string& type, GrandPotential::Constraint ctype)
    {
    constraint_types_[type] = ctype;
    }

}
