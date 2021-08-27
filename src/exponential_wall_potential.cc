#include "flyft/exponential_wall_potential.h"

namespace flyft
{

ExponentialWallPotential::ExponentialWallPotential(double origin, double normal)
    : ExponentialWallPotential(std::make_shared<ConstantDoubleParameter>(origin),normal)
    {
    }

ExponentialWallPotential::ExponentialWallPotential(std::shared_ptr<DoubleParameter> origin, double normal)
    : WallPotential(origin,normal)
    {
    compute_depends_.add(&epsilons_);
    compute_depends_.add(&kappas_);
    compute_depends_.add(&shifts_);
    }

TypeMap<double>& ExponentialWallPotential::getEpsilons()
    {
    return epsilons_;
    }

const TypeMap<double>& ExponentialWallPotential::getEpsilons() const
    {
    return epsilons_;
    }

TypeMap<double>& ExponentialWallPotential::getKappas()
    {
    return kappas_;
    }

const TypeMap<double>& ExponentialWallPotential::getKappas() const
    {
    return kappas_;
    }

TypeMap<double>& ExponentialWallPotential::getShifts()
    {
    return shifts_;
    }

const TypeMap<double>& ExponentialWallPotential::getShifts() const
    {
    return shifts_;
    }

ExponentialWallPotential::Function ExponentialWallPotential::makePotentialFunction(std::shared_ptr<State> state, const std::string& type)
    {
    const double x0 = origin_->evaluate(state) + shifts_(type);
    const auto normal = normal_->evaluate(state);
    const auto epsilon = epsilons_(type);
    const auto kappa = kappas_(type);
    return Function(x0,normal,epsilon,kappa);
    }

}
