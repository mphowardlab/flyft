#include "flyft/linear_potential.h"

namespace flyft
{

LinearPotential::LinearPotential()
    {
    compute_depends_.add(&xs_);
    compute_depends_.add(&ys_);
    compute_depends_.add(&slopes_);
    }

void LinearPotential::potential(std::shared_ptr<Field> V,
                                const std::string& type,
                                std::shared_ptr<State> state)
    {
    const auto x0 = xs_(type);
    const auto y0 = ys_(type);
    const auto m = slopes_(type);

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

}
