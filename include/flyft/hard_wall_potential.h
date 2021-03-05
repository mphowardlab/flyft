#ifndef FLYFT_HARD_WALL_POTENTIAL_H_
#define FLYFT_HARD_WALL_POTENTIAL_H_

#include "flyft/external_potential.h"

#include <memory>
#include <string>

namespace flyft
{

class HardWallPotential : public ExternalPotential
    {
    public:
        HardWallPotential() = delete;
        HardWallPotential(double origin, bool positive_normal);

        void compute(std::shared_ptr<State> state) override;

        const TypeMap<double>& getDiameters();
        double getDiameter(const std::string& type) const;
        void setDiameters(const TypeMap<double>& diameters);
        void setDiameter(const std::string& type, double diameter);

        double getOrigin() const;
        void setOrigin(double origin);

        bool hasPositiveNormal() const;
        void setPositiveNormal(bool positive_normal);

    protected:
        void potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state) override;

    private:
        TypeMap<double> diameters_;
        double origin_;
        bool positive_normal_;
    };

}

#endif // FLYFT_HARD_WALL_POTENTIAL_H_
