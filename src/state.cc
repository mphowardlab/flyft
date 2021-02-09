#include "flyft/state.h"

namespace flyft
{

State::State(std::shared_ptr<const Mesh> mesh, int num_fields)
    : State(mesh, std::vector<double>(num_fields,0.0))
    {}

State::State(std::shared_ptr<const Mesh> mesh, const std::vector<double>& Ns)
    : mesh_(mesh)
    {
    num_fields_ = static_cast<int>(Ns.size());

    fields_.resize(num_fields_);
    for (auto& f : fields_)
        {
        f = std::make_shared<Field>(mesh_->shape());
        }

    diameters_ = std::vector<double>(num_fields_,0.0);
    ideal_volumes_ = std::vector<double>(num_fields_,1.0);
    Ns_ = Ns;
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
    if (diameter >= 0.)
        {
        diameters_[idx] = diameter;
        }
    else
        {
        // error: invalid diameter
        }
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
    if (ideal_volume > 0.)
        {
        ideal_volumes_[idx] = ideal_volume;
        }
    else
        {
        // error: invalid volume
        }
    }

const std::vector<double>& State::getNs()
    {
    return Ns_;
    }

double State::getN(int idx) const
    {
    return Ns_[idx];
    }

void State::setNs(const std::vector<double>& Ns)
    {
    if (static_cast<int>(Ns.size()) != getNumFields())
        {
        // error: size must match
        }

    for (int idx=0; idx < getNumFields(); ++idx)
        {
        setN(idx, Ns[idx]);
        }
    }

void State::setN(int idx, double N)
    {
    if (N > 0.)
        {
        Ns_[idx] = N;
        }
    else
        {
        // error: invalid volume
        }
    }

}
