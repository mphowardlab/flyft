#include "flyft/hard_wall_potential.h"

#include <cmath>

namespace flyft
{

HardWallPotential::HardWallPotential(double origin, double normal)
    : HardWallPotential(std::make_shared<ConstantDoubleParameter>(origin),normal)
    {
    }

HardWallPotential::HardWallPotential(std::shared_ptr<DoubleParameter> origin, double normal)
    : WallPotential(origin,normal)
    {
    compute_depends_.add(&diameters_);
    }

TypeMap<double>& HardWallPotential::getDiameters()
    {
    return diameters_;
    }

const TypeMap<double>& HardWallPotential::getDiameters() const
    {
    return diameters_;
    }


HardWallPotential::Function HardWallPotential::makePotentialFunction(std::shared_ptr<State> state, const std::string& type)
    {
    const auto x0 = origin_->evaluate(state);
    const auto normal = normal_->evaluate(state);
    const auto diameter = diameters_(type);
    return Function(x0,normal,diameter);
    }

}
