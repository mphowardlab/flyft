#include "flyft/wall_potential.h"

namespace flyft
{

WallPotential::WallPotential(double origin, double normal)
    : WallPotential(std::make_shared<ConstantDoubleParameter>(origin),normal)
    {
    }

WallPotential::WallPotential(std::shared_ptr<DoubleParameter> origin, double normal)
    {
    setOrigin(origin);
    setNormal(normal);
    }

void WallPotential::compute(std::shared_ptr<State> state, bool compute_value)
    {
    const auto x0 = origin_->evaluate(state);
    if (x0 < 0 || x0 > state->getMesh()->full()->L())
        {
        // origin out of bounds
        }
    ExternalPotential::compute(state,compute_value);
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
    if (origin_)
        {
        compute_depends_.remove(origin_->id());
        }
    origin_ = std::make_shared<ConstantDoubleParameter>(origin);
    compute_depends_.add(origin_.get());
    }

void WallPotential::setOrigin(std::shared_ptr<DoubleParameter> origin)
    {
    if (origin_)
        {
        compute_depends_.remove(origin_->id());
        }
    origin_ = origin;
    compute_depends_.add(origin_.get());
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
    token_.stage();
    }

}
