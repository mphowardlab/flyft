#ifndef FLYFT_LENNARD_JONES_93_WALL_POTENTIAL_H_
#define FLYFT_LENNARD_JONES_93_WALL_POTENTIAL_H_

#include "flyft/field.h"
#include "flyft/state.h"
#include "flyft/type_map.h"
#include "flyft/wall_potential.h"

#include <memory>
#include <string>

namespace flyft
{

class LennardJones93WallPotentialFunction : public WallPotentialFunction
    {
    public:
        LennardJones93WallPotentialFunction(double origin, double normal, double epsilon, double sigma, double cutoff, bool shift)
            : WallPotentialFunction(origin,normal), epsilon_(epsilon), sigma_(sigma), cutoff_(cutoff)
            {
            energy_shift_ = (shift) ? computeEnergy(cutoff_) : 0.0;
            }

        double operator()(double x) const
            {
            const double dx = distance(x);
            double energy;
            if (dx <= 0)
                {
                energy = std::numeric_limits<double>::infinity();
                }
            else if (dx < cutoff_)
                {
                energy = computeEnergy(dx)-energy_shift_;
                }
            else
                {
                energy = 0.0;
                }
            return energy;
            }

    private:
        double epsilon_;
        double sigma_;
        double cutoff_;
        double energy_shift_;

        double computeEnergy(double dx) const
            {
            const double idx = sigma_/dx;
            const double idx2 = idx*idx;
            const double idx3 = idx2*idx;
            const double idx9 = idx3*idx3*idx3;
            return epsilon_*((2./15.)*idx9-idx3);
            }
    };

class LennardJones93WallPotential : public WallPotential<LennardJones93WallPotentialFunction>
    {
    public:
        LennardJones93WallPotential() = delete;
        LennardJones93WallPotential(double origin, double normal);
        LennardJones93WallPotential(std::shared_ptr<DoubleParameter> origin, double normal);

        TypeMap<double>& getEpsilons();
        const TypeMap<double>& getEpsilons() const;

        TypeMap<double>& getSigmas();
        const TypeMap<double>& getSigmas() const;

        TypeMap<double>& getCutoffs();
        const TypeMap<double>& getCutoffs() const;

        TypeMap<bool>& getShifts();
        const TypeMap<bool>& getShifts() const;

    protected:
        Function makePotentialFunction(std::shared_ptr<State> state, const std::string& type) override;

    private:
        TypeMap<double> epsilons_;
        TypeMap<double> sigmas_;
        TypeMap<double> cutoffs_;
        TypeMap<bool> shifts_;
    };

}

#endif // FLYFT_LENNARD_JONES_93_WALL_POTENTIAL_H_
