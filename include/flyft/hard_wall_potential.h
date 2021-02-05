#ifndef FLYFT_HARD_WALL_POTENTIAL_H_
#define FLYFT_HARD_WALL_POTENTIAL_H_

#include "flyft/external_potential.h"

namespace flyft
{

class HardWallPotential : public ExternalPotential
    {
    public:
        HardWallPotential() = delete;
        HardWallPotential(double origin, bool positive_normal, const std::vector<double>& diameters);

        void compute(std::shared_ptr<State> state) override;

        double getOrigin() const;
        void setOrigin(double origin);

        bool hasPositiveNormal() const;
        void setPositiveNormal(bool positive_normal);

        const std::vector<double>& getDiameters();
        double getDiameter(int idx) const;
        void setDiameters(const std::vector<double>& diameters);
        void setDiameter(int idx, double diameter);

    protected:
        void potential(std::shared_ptr<Field> V, int idx, std::shared_ptr<const Mesh> mesh) override;

    private:
        double origin_;
        bool positive_normal_;
        std::vector<double> diameters_;

        bool check_diameters_;
        bool checkDiameters() const;
    };

}

#endif // FLYFT_HARD_WALL_POTENTIAL_H_
