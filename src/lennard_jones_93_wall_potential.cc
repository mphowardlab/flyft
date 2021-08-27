#include "flyft/lennard_jones_93_wall_potential.h"

#include <cmath>

namespace flyft
{

LennardJones93WallPotential::LennardJones93WallPotential(double origin, double normal)
    : LennardJones93WallPotential(std::make_shared<ConstantDoubleParameter>(origin),normal)
    {
    }

LennardJones93WallPotential::LennardJones93WallPotential(std::shared_ptr<DoubleParameter> origin, double normal)
    : WallPotential(origin,normal)
    {
    compute_depends_.add(&epsilons_);
    compute_depends_.add(&sigmas_);
    compute_depends_.add(&cutoffs_);
    compute_depends_.add(&shifts_);
    }

TypeMap<double>& LennardJones93WallPotential::getEpsilons()
    {
    return epsilons_;
    }

const TypeMap<double>& LennardJones93WallPotential::getEpsilons() const
    {
    return epsilons_;
    }

TypeMap<double>& LennardJones93WallPotential::getSigmas()
    {
    return sigmas_;
    }

const TypeMap<double>& LennardJones93WallPotential::getSigmas() const
    {
    return sigmas_;
    }

TypeMap<double>& LennardJones93WallPotential::getCutoffs()
    {
    return cutoffs_;
    }

const TypeMap<double>& LennardJones93WallPotential::getCutoffs() const
    {
    return cutoffs_;
    }

TypeMap<bool>& LennardJones93WallPotential::getShifts()
    {
    return shifts_;
    }

const TypeMap<bool>& LennardJones93WallPotential::getShifts() const
    {
    return shifts_;
    }

LennardJones93WallPotential::Function LennardJones93WallPotential::makePotentialFunction(std::shared_ptr<State> state, const std::string& type)
    {
    const auto x0 = origin_->evaluate(state);
    const auto normal = normal_->evaluate(state);
    const auto epsilon = epsilons_(type);
    const auto sigma = sigmas_(type);
    const auto cutoff = cutoffs_(type);
    bool shift = shifts_(type);
    return Function(x0,normal,epsilon,sigma,cutoff,shift);
    }

}
