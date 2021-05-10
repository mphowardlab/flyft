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
        using WallPotential::WallPotential;

        const TypeMap<double>& getEpsilons();
        double getEpsilon(const std::string& type) const;
        void setEpsilons(const TypeMap<double>& epsilons);
        void setEpsilon(const std::string& type, double epsilon);

        const TypeMap<double>& getKappas();
        double getKappa(const std::string& type) const;
        void setKappas(const TypeMap<double>& kappas);
        void setKappa(const std::string& type, double kappa);

        const TypeMap<double>& getShifts();
        double getShift(const std::string& type) const;
        void setShifts(const TypeMap<double>& shifts);
        void setShift(const std::string& type, double shift);

        void potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state) override;

    private:
        TypeMap<double> epsilons_;
        TypeMap<double> kappas_;
        TypeMap<double> shifts_;
    };

}

#endif // FLYFT_EXPONENTIAL_WALL_POTENTIAL_H_
