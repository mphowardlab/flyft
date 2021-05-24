#include "flyft/lennard_jones_93_wall_potential.h"

#include <cmath>

namespace flyft
{

void LennardJones93WallPotential::potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state)
    {
    // get potential parameters for this type
    const auto epsilon = epsilons_.at(type);
    const auto sigma = sigmas_.at(type);
    const auto cutoff = cutoffs_.at(type);
    const auto x0 = origin_->evaluate(state);

    // precompute energy shift
    bool shift = shifts_.at(type);
    double energy_shift;
    if (shift)
        {
        const double icut = sigma/cutoff;
        const double icut2 = icut*icut;
        const double icut3 = icut2*icut;
        const double icut9 = icut3*icut3*icut3;
        energy_shift = epsilon*((2./15.)*icut9-icut3);
        }
    else
        {
        energy_shift = 0.;
        }

    const auto mesh = *(state->getMesh());
    auto data = V->data();

    #pragma omp parallel for default(none) shared(mesh,data,epsilon,sigma,cutoff,x0,normal_,energy_shift)
    for (int idx=0; idx < mesh.shape(); ++idx)
        {
        const auto x = mesh.coordinate(idx);
        const double dx = normal_*(x-x0);

        // get 9 & 3 parts scaled by sigma
        double energy;
        if (dx <= 0)
            {
            energy = std::numeric_limits<double>::infinity();
            }
        else if (dx < cutoff)
            {
            const double idx = sigma/dx;
            const double idx2 = idx*idx;
            const double idx3 = idx2*idx;
            const double idx9 = idx3*idx3*idx3;
            energy = epsilon*((2./15.)*idx9-idx3)-energy_shift;
            }
        else
            {
            energy = 0.0;
            }
        data[idx] = energy;
        }
    }

const TypeMap<double>& LennardJones93WallPotential::getEpsilons()
    {
    return epsilons_;
    }

double LennardJones93WallPotential::getEpsilon(const std::string& type) const
    {
    return epsilons_.at(type);
    }

void LennardJones93WallPotential::setEpsilons(const TypeMap<double>& epsilons)
    {
    epsilons_ = TypeMap<double>(epsilons);
    }

void LennardJones93WallPotential::setEpsilon(const std::string& type, double epsilon)
    {
    epsilons_[type] = epsilon;
    }

const TypeMap<double>& LennardJones93WallPotential::getSigmas()
    {
    return sigmas_;
    }

double LennardJones93WallPotential::getSigma(const std::string& type) const
    {
    return sigmas_.at(type);
    }

void LennardJones93WallPotential::setSigmas(const TypeMap<double>& sigmas)
    {
    sigmas_ = TypeMap<double>(sigmas);
    }

void LennardJones93WallPotential::setSigma(const std::string& type, double sigma)
    {
    sigmas_[type] = sigma;
    }

const TypeMap<double>& LennardJones93WallPotential::getCutoffs()
    {
    return cutoffs_;
    }

double LennardJones93WallPotential::getCutoff(const std::string& type) const
    {
    return cutoffs_.at(type);
    }

void LennardJones93WallPotential::setCutoffs(const TypeMap<double>& cutoffs)
    {
    cutoffs_ = TypeMap<double>(cutoffs);
    }

void LennardJones93WallPotential::setCutoff(const std::string& type, double cutoff)
    {
    cutoffs_[type] = cutoff;
    }

const TypeMap<bool>& LennardJones93WallPotential::getShifts()
    {
    return shifts_;
    }

bool LennardJones93WallPotential::getShift(const std::string& type) const
    {
    return shifts_.at(type);
    }

void LennardJones93WallPotential::setShifts(const TypeMap<bool>& shifts)
    {
    shifts_ = TypeMap<bool>(shifts);
    }

void LennardJones93WallPotential::setShift(const std::string& type, bool shift)
    {
    shifts_[type] = shift;
    }

}
