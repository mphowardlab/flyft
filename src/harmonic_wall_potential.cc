#include "flyft/harmonic_wall_potential.h"

namespace flyft
    {

HarmonicWallPotential::HarmonicWallPotential(double origin, double normal)
    : HarmonicWallPotential(std::make_shared<ConstantDoubleParameter>(origin), normal)
    {
    }

HarmonicWallPotential::HarmonicWallPotential(std::shared_ptr<DoubleParameter> origin, double normal)
    : WallPotential(origin, normal)
    {
    compute_depends_.add(&spring_constants_);
    compute_depends_.add(&shifts_);
    }

TypeMap<double>& HarmonicWallPotential::getSpringConstants()
    {
    return spring_constants_;
    }

const TypeMap<double>& HarmonicWallPotential::getSpringConstants() const
    {
    return spring_constants_;
    }

TypeMap<double>& HarmonicWallPotential::getShifts()
    {
    return shifts_;
    }

const TypeMap<double>& HarmonicWallPotential::getShifts() const
    {
    return shifts_;
    }

HarmonicWallPotential::Function
HarmonicWallPotential::makePotentialFunction(std::shared_ptr<State> state, const std::string& type)
    {
    const double x0 = origin_->evaluate(state) + shifts_(type);
    const auto normal = normal_->evaluate(state);
    const auto k = spring_constants_(type);
    return Function(x0, normal, k);
    }

    } // namespace flyft
