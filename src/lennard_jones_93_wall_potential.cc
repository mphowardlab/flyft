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

void LennardJones93WallPotential::potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state)
    {
    // get potential parameters for this type
    const auto epsilon = epsilons_(type);
    const auto sigma = sigmas_(type);
    const auto cutoff = cutoffs_(type);
    const auto x0 = origin_->evaluate(state);
    const auto normal = normal_->evaluate(state);

    // precompute energy shift
    bool shift = shifts_(type);
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

    const auto mesh = *state->getMesh()->local();
    auto data = V->view();
    #ifdef FLYFT_OPENMP
    #pragma omp parallel for schedule(static) default(none) firstprivate(epsilon,sigma,cutoff,x0,energy_shift,normal,mesh) shared(data)
    #endif
    for (int idx=0; idx < mesh.shape(); ++idx)
        {
        const auto x = mesh.coordinate(idx);
        const double dx = normal*(x-x0);

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
        data(idx) = energy;
        }
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

}
