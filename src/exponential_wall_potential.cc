#include "flyft/exponential_wall_potential.h"

namespace flyft
{

void ExponentialWallPotential::potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state)
    {
    // get potential parameters for this type
    const auto epsilon = epsilons_(type);
    const auto kappa = kappas_(type);
    const auto shift = shifts_(type);
    const double x0 = origin_->evaluate(state) + shift;
    const auto normal = normal_;

    const auto mesh = *state->getMesh()->local();
    auto data = V->view();
    #ifdef FLYFT_OPENMP
    #pragma omp parallel for schedule(static) default(none) firstprivate(epsilon,kappa,shift,x0,normal,mesh) shared(data)
    #endif
    for (int idx=0; idx < mesh.shape(); ++idx)
        {
        const auto x = mesh.coordinate(idx);
        const double dx = normal*(x-x0);
        data(idx) = epsilon*std::exp(-kappa*dx);
        }
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

}
