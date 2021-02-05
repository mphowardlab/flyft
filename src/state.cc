#include "flyft/state.h"

namespace flyft
{

State::State(std::shared_ptr<const Mesh> mesh, int num_fields)
    : mesh_(mesh), num_fields_(num_fields), fields_(num_fields), diameters_(num_fields),
      ideal_volumes_(num_fields)
    {
    for (auto& f : fields_)
        {
        f = std::make_shared<Field>(mesh_->shape());
        }
    std::fill(diameters_.begin(), diameters_.end(), 0.0);
    std::fill(ideal_volumes_.begin(), ideal_volumes_.end(), 1.0);
    }

std::shared_ptr<const Mesh> State::getMesh() const
    {
    return mesh_;
    }

int State::getNumFields() const
    {
    return num_fields_;
    }

const std::vector<std::shared_ptr<Field>>& State::getFields()
    {
    return fields_;
    }

std::shared_ptr<Field> State::getField(int idx)
    {
    return fields_[idx];
    }

std::shared_ptr<const Field> State::getField(int idx) const
    {
    return fields_[idx];
    }

const std::vector<double>& State::getDiameters()
    {
    return diameters_;
    }

double State::getDiameter(int idx) const
    {
    return diameters_[idx];
    }

void State::setDiameters(const std::vector<double>& diameters)
    {
    if (static_cast<int>(diameters.size()) != getNumFields())
        {
        // error: size must match
        }

    for (int idx=0; idx < getNumFields(); ++idx)
        {
        setDiameter(idx, diameters[idx]);
        }
    }

void State::setDiameter(int idx, double diameter)
    {
    if (checkDiameter(diameter))
        {
        diameters_[idx] = diameter;
        }
    else
        {
        // error: invalid diameter
        }
    }

bool State::checkDiameter(double diameter) const
    {
    return diameter >= 0.;
    }

const std::vector<double>& State::getIdealVolumes()
    {
    return ideal_volumes_;
    }

double State::getIdealVolume(int idx) const
    {
    return ideal_volumes_[idx];
    }

void State::setIdealVolumes(const std::vector<double>& ideal_volumes)
    {
    if (static_cast<int>(ideal_volumes.size()) != getNumFields())
        {
        // error: size must match
        }

    for (int idx=0; idx < getNumFields(); ++idx)
        {
        setIdealVolume(idx, ideal_volumes[idx]);
        }
    }

void State::setIdealVolume(int idx, double ideal_volume)
    {
    if (checkIdealVolume(ideal_volume))
        {
        ideal_volumes_[idx] = ideal_volume;
        }
    else
        {
        // error: invalid diameter
        }
    }

bool State::checkIdealVolume(double ideal_volume) const
    {
    return ideal_volume > 0.;
    }

}
