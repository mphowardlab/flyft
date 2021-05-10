#include "flyft/wall_potential.h"

namespace flyft
{

WallPotential::WallPotential(double origin, double normal)
    {
    setOrigin(origin);
    setNormal(normal);
    }

WallPotential::WallPotential(std::shared_ptr<DoubleParameter> origin, double normal)
    {
    setOrigin(origin);
    setNormal(normal);
    }

void WallPotential::compute(std::shared_ptr<State> state)
    {
    const auto x0 = origin_->evaluate(state);
    if (x0 < 0 || x0 > state->getMesh()->L())
        {
        // origin out of bounds
        }
    ExternalPotential::compute(state);
    }

std::shared_ptr<DoubleParameter> WallPotential::getOrigin()
    {
    return origin_;
    }

std::shared_ptr<const DoubleParameter> WallPotential::getOrigin() const
    {
    return origin_;
    }

void WallPotential::setOrigin(double origin)
    {
    origin_ = std::make_shared<ConstantDoubleParameter>(origin);
    }

void WallPotential::setOrigin(std::shared_ptr<DoubleParameter> origin)
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
