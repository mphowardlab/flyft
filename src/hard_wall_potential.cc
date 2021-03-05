#include "flyft/hard_wall_potential.h"

#include <cmath>

namespace flyft
{

HardWallPotential::HardWallPotential(double origin, bool positive_normal)
    : origin_(origin), positive_normal_(positive_normal)
    {}

void HardWallPotential::compute(std::shared_ptr<State> state)
    {
    if (origin_ < 0 || origin_ > state->getMesh()->L())
        {
        // origin out of bounds
        }
    ExternalPotential::compute(state);
    }

void HardWallPotential::potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state)
    {
    // edge where sphere contacts the wall
    const double R = 0.5*diameters_.at(type);
    const double edge = origin_ + ((positive_normal_) ? R : -R);

    auto mesh = state->getMesh();
    auto data = V->data();
    for (int idx=0; idx < mesh->shape(); ++idx)
        {
        const auto x = mesh->coordinate(idx);
        data[idx] = ((positive_normal_ && x < edge) || (!positive_normal_ && x > edge)) ? std::numeric_limits<double>::infinity() : 0.0;
        }
    }

const TypeMap<double>& HardWallPotential::getDiameters()
    {
    return diameters_;
    }

double HardWallPotential::getDiameter(const std::string& type) const
    {
    return diameters_.at(type);
    }

void HardWallPotential::setDiameters(const TypeMap<double>& diameters)
    {
    diameters_ = TypeMap<double>(diameters);
    }

void HardWallPotential::setDiameter(const std::string& type, double diameter)
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

double HardWallPotential::getOrigin() const
    {
    return origin_;
    }

void HardWallPotential::setOrigin(double origin)
    {
    origin_ = origin;
    }

bool HardWallPotential::hasPositiveNormal() const
    {
    return positive_normal_;
    }

void HardWallPotential::setPositiveNormal(bool positive_normal)
    {
    positive_normal_ = positive_normal;
    }

}
