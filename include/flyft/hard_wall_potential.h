#ifndef FLYFT_HARD_WALL_POTENTIAL_H_
#define FLYFT_HARD_WALL_POTENTIAL_H_

#include "flyft/field.h"
#include "flyft/state.h"
#include "flyft/type_map.h"
#include "flyft/wall_potential.h"

#include <memory>
#include <string>

namespace flyft
{

class HardWallPotentialFunction : public WallPotentialFunction
    {
    public:
        HardWallPotentialFunction(double origin, double normal, double diameter)
            : WallPotentialFunction(origin,normal), R_(0.5*diameter)
            {}

        double operator()(double x) const
            {
            const double dx = distance(x);
            return (dx < R_) ? std::numeric_limits<double>::infinity() : 0.0;
            }

    private:
        double R_;
    };

class HardWallPotential : public WallPotential<HardWallPotentialFunction>
    {
    public:
        HardWallPotential() = delete;
        HardWallPotential(double origin, double normal);
        HardWallPotential(std::shared_ptr<DoubleParameter> origin, double normal);

        TypeMap<double>& getDiameters();
        const TypeMap<double>& getDiameters() const;

    protected:
        Function makePotentialFunction(std::shared_ptr<State> state, const std::string& type) override;

    private:
        TypeMap<double> diameters_;
    };

}

#endif // FLYFT_HARD_WALL_POTENTIAL_H_
