#ifndef FLYFT_HARMONIC_WALL_POTENTIAL_H_
#define FLYFT_HARMONIC_WALL_POTENTIAL_H_

#include "flyft/field.h"
#include "flyft/state.h"
#include "flyft/type_map.h"
#include "flyft/wall_potential.h"

#include <memory>
#include <string>

namespace flyft
{

class HarmonicWallPotentialFunction : public WallPotentialFunction
    {
    public:
        HarmonicWallPotentialFunction(double origin, double normal, double k)
            : WallPotentialFunction(origin,normal), k_(k)
            {}

        double operator()(double x) const
            {
            const double dx = distance(x);
            return (dx < 0) ? 0.5*k_*dx*dx : 0.0;
            }

    private:
        double k_;
    };

class HarmonicWallPotential : public WallPotential<HarmonicWallPotentialFunction>
    {
    public:
        HarmonicWallPotential() = delete;
        HarmonicWallPotential(double origin, double normal);
        HarmonicWallPotential(std::shared_ptr<DoubleParameter> origin, double normal);

        TypeMap<double>& getSpringConstants();
        const TypeMap<double>& getSpringConstants() const;

        TypeMap<double>& getShifts();
        const TypeMap<double>& getShifts() const;

    protected:
        Function makePotentialFunction(std::shared_ptr<State> state, const std::string& type) override;

    private:
        TypeMap<double> spring_constants_;
        TypeMap<double> shifts_;
    };

}

#endif // FLYFT_HARMONIC_WALL_POTENTIAL_H_
