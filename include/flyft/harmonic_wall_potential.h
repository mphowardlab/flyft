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

class HarmonicWallPotential : public WallPotential
    {
    public:
        HarmonicWallPotential() = delete;
        HarmonicWallPotential(double origin, double normal);
        HarmonicWallPotential(std::shared_ptr<DoubleParameter> origin, double normal);

        TypeMap<double>& getSpringConstants();
        const TypeMap<double>& getSpringConstants() const;

        TypeMap<double>& getShifts();
        const TypeMap<double>& getShifts() const;

        void potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state) override;

    private:
        TypeMap<double> spring_constants_;
        TypeMap<double> shifts_;
    };

}

#endif // FLYFT_HARMONIC_WALL_POTENTIAL_H_
