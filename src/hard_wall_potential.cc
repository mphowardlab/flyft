#include "flyft/hard_wall_potential.h"

#include <cmath>

namespace flyft
{

HardWallPotential::HardWallPotential(double origin, double normal)
    : origin_(origin)
    {
    setNormal(normal);
    }

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
    const double edge = origin_ + normal_*R;

    auto mesh = state->getMesh();
    auto data = V->data();
    for (int idx=0; idx < mesh->shape(); ++idx)
        {
        const auto x = mesh->coordinate(idx);
        data[idx] = (normal_*(x-edge) < 0) ? std::numeric_limits<double>::infinity() : 0.0;
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

double HardWallPotential::getNormal() const
    {
    return normal_;
    }

void HardWallPotential::setNormal(double normal)
    {
    if (normal > 0)
        {
        normal_ = 1.0;
        }
    else if (normal < 0)
        {
        normal_ = -1.0;
        }
    else
        {
        // error: normal must be nonzero
        }
    }

}
