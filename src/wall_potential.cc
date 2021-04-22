#include "flyft/wall_potential.h"

namespace flyft
{

WallPotential::WallPotential(double origin, double normal)
    : origin_(origin)
    {
    setNormal(normal);
    }

void WallPotential::compute(std::shared_ptr<State> state)
    {
    if (origin_ < 0 || origin_ > state->getMesh()->L())
        {
        // origin out of bounds
        }
    ExternalPotential::compute(state);
    }

double WallPotential::getOrigin() const
    {
    return origin_;
    }

void WallPotential::setOrigin(double origin)
    {
    origin_ = origin;
    }

double WallPotential::getNormal() const
    {
    return normal_;
    }

void WallPotential::setNormal(double normal)
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
