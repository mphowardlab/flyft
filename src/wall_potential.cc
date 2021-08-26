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

bool WallPotential::setup(std::shared_ptr<State> state, bool compute_value)
    {
    const auto x0 = origin_->evaluate(state);
    if (x0 < 0 || x0 > state->getMesh()->full()->L())
        {
        // origin out of bounds
        }
    return ExternalPotential::setup(state,compute_value);
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
    setOrigin(std::make_shared<ConstantDoubleParameter>(origin));
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

std::shared_ptr<DoubleParameter> WallPotential::getNormal()
    {
    return normal_;
    }

std::shared_ptr<const DoubleParameter> WallPotential::getNormal() const
    {
    return normal_;
    }

void WallPotential::setNormal(double normal)
    {
    setNormal(std::make_shared<ConstantDoubleParameter>(normal));
    }


void WallPotential::setNormal(std::shared_ptr<DoubleParameter> normal)
    {
    if (normal_)
        {
        compute_depends_.remove(normal_->id());
        }
    normal_ = normal;
    compute_depends_.add(normal_.get());
    }

double WallPotential::computeNormal(std::shared_ptr<State> state)
    {
    double n = normal_->evaluate(state);
    if (n > 0)
        {
        n = 1.0;
        }
    else if (n < 0)
        {
        n = -1.0;
        }
    else
        {
        throw std::runtime_error("Normal must be nonzero");
        }
    return n;
    }

}
