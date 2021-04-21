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

class HardWallPotential : public WallPotential
    {
    public:
        HardWallPotential() = delete;
        HardWallPotential(double origin, double normal);

        void compute(std::shared_ptr<State> state) override;

        const TypeMap<double>& getDiameters();
        double getDiameter(const std::string& type) const;
        void setDiameters(const TypeMap<double>& diameters);
        void setDiameter(const std::string& type, double diameter);

        void potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state) override;

    private:
        TypeMap<double> diameters_;
    };

}

#endif // FLYFT_HARD_WALL_POTENTIAL_H_
