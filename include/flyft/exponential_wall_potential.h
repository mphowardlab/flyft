#ifndef FLYFT_EXPONENTIAL_WALL_POTENTIAL_H_
#define FLYFT_EXPONENTIAL_WALL_POTENTIAL_H_

#include "flyft/field.h"
#include "flyft/state.h"
#include "flyft/type_map.h"
#include "flyft/wall_potential.h"

#include <memory>
#include <string>

namespace flyft
{

class ExponentialWallPotential : public WallPotential
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

        void potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state) override;

    private:
        TypeMap<double> epsilons_;
        TypeMap<double> kappas_;
        TypeMap<double> shifts_;
    };

}

#endif // FLYFT_EXPONENTIAL_WALL_POTENTIAL_H_
