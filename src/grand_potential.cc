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
    ideal_->compute(state);
    value_ = ideal_->getValue();

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

    // sum up contributions
    for (const auto& t : state->getTypes())
        {
        auto d = derivatives_[t]->data();
        auto did = ideal_->getDerivative(t)->data();
        auto dex = (excess_) ? excess_->getDerivative(t)->data() : nullptr;
        auto dext = (external_) ? external_->getDerivative(t)->data() : nullptr;
        for (int idx=0; idx < mesh->shape(); ++idx)
            {
            d[idx] = did[idx];
            if (dex) d[idx] += dex[idx];
            if (dext) d[idx] += dext[idx];
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

std::shared_ptr<const IdealGasFunctional> GrandPotential::getIdealGasFunctional() const
    {
    return ideal_;
    }

void GrandPotential::setIdealGasFunctional(std::shared_ptr<IdealGasFunctional> ideal)
    {
    if (ideal)
        {
        ideal_ = ideal;
        }
    else
        {
        // can't set with nullptr
        }
    }

std::shared_ptr<FreeEnergyFunctional> GrandPotential::getExcessFunctional()
    {
    return excess_;
    }

std::shared_ptr<const FreeEnergyFunctional> GrandPotential::getExcessFunctional() const
    {
    return excess_;
    }

void GrandPotential::setExcessFunctional(std::shared_ptr<FreeEnergyFunctional> excess)
    {
    excess_ = excess;
    }

std::shared_ptr<FreeEnergyFunctional> GrandPotential::getExternalPotential()
    {
    return external_;
    }

std::shared_ptr<const FreeEnergyFunctional> GrandPotential::getExternalPotential() const
    {
    return external_;
    }

void GrandPotential::setExternalPotential(std::shared_ptr<FreeEnergyFunctional> external)
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
