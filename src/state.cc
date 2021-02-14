#include "flyft/state.h"

namespace flyft
{

State::State(std::shared_ptr<const Mesh> mesh, const std::string& type)
    : State(mesh, std::vector<std::string>({type}))
    {}

State::State(std::shared_ptr<const Mesh> mesh, const std::vector<std::string>& types)
    : mesh_(mesh), types_(types)
    {
    for (const auto& t : types_)
        {
        fields_[t] = std::make_shared<Field>(mesh_->shape());
        setDiameter(t,0.0);
        }
    }

std::shared_ptr<const Mesh> State::getMesh() const
    {
    return mesh_;
    }

int State::getNumFields() const
    {
    return static_cast<int>(types_.size());
    }

const std::vector<std::string>& State::getTypes()
    {
    return types_;
    }

const std::string State::getType(int idx) const
    {
    return types_.at(idx);
    }

int State::getTypeIndex(const std::string& type) const
    {
    const auto it = std::find(types_.begin(), types_.end(), type);
    if (it == types_.end())
        {
        // error: type not found
        }
    return (it - types_.begin());
    }

const TypeMap<std::shared_ptr<Field>>& State::getFields()
    {
    return fields_;
    }

std::shared_ptr<Field> State::getField(const std::string& type)
    {
    return fields_[type];
    }

std::shared_ptr<const Field> State::getField(const std::string& type) const
    {
    return fields_.at(type);
    }

const TypeMap<double>& State::getDiameters()
    {
    return diameters_;
    }

double State::getDiameter(const std::string& type) const
    {
    return diameters_.at(type);
    }

void State::setDiameters(const TypeMap<double>& diameters)
    {
    diameters_ = TypeMap<double>(diameters);
    }

void State::setDiameter(const std::string& type, double diameter)
    {
    if (diameter >= 0.)
        {
        diameters_[type] = diameter;
        }
    else
        {
        // error: invalid diameter
        }
    }

const TypeMap<double>& State::getIdealVolumes()
    {
    return ideal_volumes_;
    }

double State::getIdealVolume(const std::string& type) const
    {
    return ideal_volumes_.at(type);
    }

void State::setIdealVolumes(const TypeMap<double>& ideal_volumes)
    {
    ideal_volumes_ = TypeMap<double>(ideal_volumes);
    }

void State::setIdealVolume(const std::string& type, double ideal_volume)
    {
    if (ideal_volume > 0.)
        {
        ideal_volumes_[type] = ideal_volume;
        }
    else
        {
        // error: invalid volume
        }
    }

const TypeMap<double>& State::getConstraints()
    {
    for (const auto& t : types_)
        {
        checkConstraint(t);
        }
    return constraints_;
    }

double State::getConstraint(const std::string& type)
    {
    checkConstraint(type);
    return constraints_[type];
    }

const TypeMap<State::Constraint>& State::getConstraintTypes()
    {
    for (const auto& t : types_)
        {
        checkConstraint(t);
        }
    return constraint_types_;
    }

State::Constraint State::getConstraintType(const std::string& type)
    {
    checkConstraint(type);
    return constraint_types_[type];
    }

void State::setConstraint(const std::string& type, double value, State::Constraint ctype)
    {
    constraints_[type] = value;
    constraint_types_[type] = ctype;
    }

void State::setConstraintType(const std::string& type, State::Constraint ctype)
    {
    constraint_types_[type] = ctype;
    }

void State::checkConstraint(const std::string& type)
    {
    if (constraint_types_[type] == Constraint::compute)
        {
        auto rho = fields_[type]->data();
        double sum = 0.0;
        for (int idx=0; idx < mesh_->shape(); ++idx)
            {
            sum += rho[idx];
            }
        sum *= mesh_->step();
        setConstraint(type, sum, Constraint::N);
        }
    }

}
