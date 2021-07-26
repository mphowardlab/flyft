#include "flyft/exponential_wall_potential.h"

namespace flyft
{

void ExponentialWallPotential::potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state)
    {
    // get potential parameters for this type
    const auto epsilon = epsilons_.at(type);
    const auto kappa = kappas_.at(type);
    const auto shift = shifts_.at(type);
    const double x0 = origin_->evaluate(state) + shift;
    const auto normal = normal_;

    const auto mesh = *state->getMesh();
    auto data = V->begin();

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

const TypeMap<double>& ExponentialWallPotential::getEpsilons()
    {
    return epsilons_;
    }

double ExponentialWallPotential::getEpsilon(const std::string& type) const
    {
    return epsilons_.at(type);
    }

void ExponentialWallPotential::setEpsilons(const TypeMap<double>& epsilons)
    {
    epsilons_ = TypeMap<double>(epsilons);
    }

void ExponentialWallPotential::setEpsilon(const std::string& type, double epsilon)
    {
    epsilons_[type] = epsilon;
    }

const TypeMap<double>& ExponentialWallPotential::getKappas()
    {
    return kappas_;
    }

double ExponentialWallPotential::getKappa(const std::string& type) const
    {
    return kappas_.at(type);
    }

void ExponentialWallPotential::setKappas(const TypeMap<double>& kappas)
    {
    kappas_ = TypeMap<double>(kappas);
    }

void ExponentialWallPotential::setKappa(const std::string& type, double kappa)
    {
    kappas_[type] = kappa;
    }

const TypeMap<double>& ExponentialWallPotential::getShifts()
    {
    return shifts_;
    }

double ExponentialWallPotential::getShift(const std::string& type) const
    {
    return shifts_.at(type);
    }

void ExponentialWallPotential::setShifts(const TypeMap<double>& shifts)
    {
    shifts_ = TypeMap<double>(shifts);
    }

void ExponentialWallPotential::setShift(const std::string& type, double shift)
    {
    shifts_[type] = shift;
    }

}
