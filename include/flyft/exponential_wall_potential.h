#ifndef FLYFT_EXPONENTIAL_WALL_POTENTIAL_H_
#define FLYFT_EXPONENTIAL_WALL_POTENTIAL_H_

#include "flyft/field.h"
#include "flyft/state.h"
#include "flyft/type_map.h"
#include "flyft/wall_potential.h"

#include <cmath>
#include <memory>
#include <string>

namespace flyft
{

class ExponentialWallPotentialFunction : public WallPotentialFunction
    {
    public:
        ExponentialWallPotentialFunction(double origin, double normal, double epsilon, double kappa)
            : WallPotentialFunction(origin,normal), epsilon_(epsilon), kappa_(kappa)
            {}

        double operator()(double x) const
            {
            const double dx = distance(x);
            return epsilon_*std::exp(-kappa_*dx);
            }

    private:
        double epsilon_;
        double kappa_;
    };

class ExponentialWallPotential : public WallPotential<ExponentialWallPotentialFunction>
    {
    public:
        ExponentialWallPotential() = delete;
        ExponentialWallPotential(double origin, double normal);
        ExponentialWallPotential(std::shared_ptr<DoubleParameter> origin, double normal);

        TypeMap<double>& getEpsilons();
        const TypeMap<double>& getEpsilons() const;

        TypeMap<double>& getKappas();
        const TypeMap<double>& getKappas() const;

        TypeMap<double>& getShifts();
        const TypeMap<double>& getShifts() const;

    protected:
        Function makePotentialFunction(std::shared_ptr<State> state, const std::string& type) override;

    private:
        TypeMap<double> epsilons_;
        TypeMap<double> kappas_;
        TypeMap<double> shifts_;
    };

}

#endif // FLYFT_EXPONENTIAL_WALL_POTENTIAL_H_
