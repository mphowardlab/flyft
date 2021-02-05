#include "flyft/hard_wall_potential.h"

#include <algorithm>
#include <cmath>

namespace flyft
{

HardWallPotential::HardWallPotential(double origin,
                                                 bool positive_normal,
                                                 const std::vector<double>& diameters)
    : origin_(origin), positive_normal_(positive_normal), check_diameters_(true)
    {
    setDiameters(diameters);
    }

void HardWallPotential::compute(std::shared_ptr<State> state)
    {
    if (origin_ < 0 || origin_ > state->getMesh()->L())
        {
        // origin out of bounds
        }

    if (!checkDiameters())
        {
        // invalid diameter
        }

    ExternalPotential::compute(state);
    }

void HardWallPotential::potential(std::shared_ptr<Field> V, int idx, std::shared_ptr<const Mesh> mesh)
    {
    // edge where sphere contacts the wall
    const double R = 0.5*getDiameter(idx);
    const double edge = origin_ + ((positive_normal_) ? R : -R);

    auto data = V->data();
    for (int i=0; i < mesh->shape(); ++i)
        {
        const auto x = mesh->coordinate(i);
        data[i] = ((positive_normal_ && x < edge) || (!positive_normal_ && x > edge)) ? std::numeric_limits<double>::infinity() : 0.0;
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

const std::vector<double>& HardWallPotential::getDiameters()
    {
    return diameters_;
    }

double HardWallPotential::getDiameter(int idx) const
    {
    return diameters_[idx];
    }

void HardWallPotential::setDiameters(const std::vector<double>& diameters)
    {
    diameters_.resize(diameters.size());
    std::copy(diameters.begin(), diameters.end(), diameters_.begin());
    check_diameters_ = true;
    }

void HardWallPotential::setDiameter(int idx, double diameter)
    {
    diameters_[idx] = diameter;
    check_diameters_ = true;
    }

bool HardWallPotential::checkDiameters() const
    {
    if (!check_diameters_) return true;

    // diameters should be positive
    for (const auto& d : diameters_)
        {
        if (d < 0)
            {
            return false;
            }
        }

    return true;
    }

}
