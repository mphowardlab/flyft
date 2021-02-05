#include "flyft/hard_wall_potential.h"

#include <algorithm>
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

void HardWallPotential::potential(std::shared_ptr<Field> V, int idx, std::shared_ptr<State> state)
    {
    // edge where sphere contacts the wall
    const double R = 0.5*state->getDiameter(idx);
    const double edge = origin_ + ((positive_normal_) ? R : -R);

    auto mesh = state->getMesh();
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

}
