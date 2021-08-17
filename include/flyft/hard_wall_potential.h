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
        using WallPotential::WallPotential;

        TypeMap<double>& getDiameters();
        const TypeMap<double>& getDiameters() const;

        void potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state) override;

    private:
        TypeMap<double> diameters_;
    };

}

#endif // FLYFT_HARD_WALL_POTENTIAL_H_
