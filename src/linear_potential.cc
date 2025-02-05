#include "flyft/linear_potential.h"

namespace flyft
    {

LinearPotential::LinearPotential()
    {
    compute_depends_.add(&xs_);
    compute_depends_.add(&ys_);
    compute_depends_.add(&slopes_);
    }

TypeMap<double>& LinearPotential::getXs()
    {
    return xs_;
    }

const TypeMap<double>& LinearPotential::getXs() const
    {
    return xs_;
    }

TypeMap<double>& LinearPotential::getYs()
    {
    return ys_;
    }

const TypeMap<double>& LinearPotential::getYs() const
    {
    return ys_;
    }

TypeMap<double>& LinearPotential::getSlopes()
    {
    return slopes_;
    }

const TypeMap<double>& LinearPotential::getSlopes() const
    {
    return slopes_;
    }

LinearPotential::Function LinearPotential::makePotentialFunction(std::shared_ptr<State> /*state*/,
                                                                 const std::string& type)
    {
    const auto x0 = xs_(type);
    const auto y0 = ys_(type);
    const auto slope = slopes_(type);
    return Function(x0, y0, slope);
    }

    } // namespace flyft
