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

class LennardJones93WallPotential : public WallPotential
    {
    public:
        LennardJones93WallPotential() = delete;
        LennardJones93WallPotential(double origin, double normal);
        LennardJones93WallPotential(std::shared_ptr<DoubleParameter> origin, double normal);

        void potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state) override;

        TypeMap<double>& getEpsilons();
        const TypeMap<double>& getEpsilons() const;

        TypeMap<double>& getSigmas();
        const TypeMap<double>& getSigmas() const;

        TypeMap<double>& getCutoffs();
        const TypeMap<double>& getCutoffs() const;

        TypeMap<bool>& getShifts();
        const TypeMap<bool>& getShifts() const;

    private:
        TypeMap<double> epsilons_;
        TypeMap<double> sigmas_;
        TypeMap<double> cutoffs_;
        TypeMap<bool> shifts_;
    };

}

#endif // FLYFT_LENNARD_JONES_93_WALL_POTENTIAL_H_
