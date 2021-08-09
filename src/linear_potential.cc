#include "flyft/linear_potential.h"

namespace flyft
{

void LinearPotential::potential(std::shared_ptr<Field> V,
                                const std::string& type,
                                std::shared_ptr<State> state)
    {
    const auto x0 = xs_.at(type);
    const auto y0 = ys_.at(type);
    const auto m = slopes_.at(type);

    const auto mesh = *state->getMesh()->local();
    auto data = V->view();
    #ifdef FLYFT_OPENMP
    #pragma omp parallel for schedule(static) default(none) firstprivate(x0,y0,m,mesh) shared(data)
    #endif
    for (int idx=0; idx < mesh.shape(); ++idx)
        {
        const auto x = mesh.coordinate(idx);
        data(idx) = y0+m*(x-x0);
        }
    }

const TypeMap<double>& LinearPotential::getXs()
    {
    return xs_;
    }

double LinearPotential::getX(const std::string& type) const
    {
    return xs_.at(type);
    }

void LinearPotential::setXs(const TypeMap<double>& xs)
    {
    xs_ = TypeMap<double>(xs);
    }

void LinearPotential::setX(const std::string& type, double x)
    {
    xs_[type] = x;
    }

const TypeMap<double>& LinearPotential::getYs()
    {
    return ys_;
    }

double LinearPotential::getY(const std::string& type) const
    {
    return ys_.at(type);
    }

void LinearPotential::setYs(const TypeMap<double>& ys)
    {
    ys_ = TypeMap<double>(ys);
    }

void LinearPotential::setY(const std::string& type, double y)
    {
    ys_[type] = y;
    }

const TypeMap<double>& LinearPotential::getSlopes()
    {
    return slopes_;
    }

double LinearPotential::getSlope(const std::string& type) const
    {
    return slopes_.at(type);
    }

void LinearPotential::setSlopes(const TypeMap<double>& slopes)
    {
    slopes_ = TypeMap<double>(slopes);
    }

void LinearPotential::setSlope(const std::string& type, double slope)
    {
    slopes_[type] = slope;
    }

}
