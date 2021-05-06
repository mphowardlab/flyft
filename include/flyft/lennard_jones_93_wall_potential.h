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
        using WallPotential::WallPotential;

        const TypeMap<double>& getEpsilons();
        double getEpsilon(const std::string& type) const;
        void setEpsilons(const TypeMap<double>& epsilons);
        void setEpsilon(const std::string& type, double epsilon);

        const TypeMap<double>& getSigmas();
        double getSigma(const std::string& type) const;
        void setSigmas(const TypeMap<double>& sigmas);
        void setSigma(const std::string& type, double sigma);

        const TypeMap<double>& getCutoffs();
        double getCutoff(const std::string& type) const;
        void setCutoffs(const TypeMap<double>& cutoffs);
        void setCutoff(const std::string& type, double cutoff);

        const TypeMap<bool>& getShifts();
        bool getShift(const std::string& type) const;
        void setShifts(const TypeMap<bool>& shifts);
        void setShift(const std::string& type, bool shift);

        void potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state) override;

    private:
        TypeMap<double> epsilons_;
        TypeMap<double> sigmas_;
        TypeMap<double> cutoffs_;
        TypeMap<bool> shifts_;
    };

}

#endif // FLYFT_LENNARD_JONES_93_WALL_POTENTIAL_H_
