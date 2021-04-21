#include "flyft/wall_potential.h"

namespace flyft
{

WallPotential::WallPotential(double origin, double normal)
    : origin_(origin)
    {
    setNormal(normal);
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
