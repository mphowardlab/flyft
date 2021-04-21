#ifndef FLYFT_WALL_POTENTIAL_H_
#define FLYFT_WALL_POTENTIAL_H_

#include "flyft/external_potential.h"

#include <memory>
#include <string>

namespace flyft
{

class WallPotential : public ExternalPotential
    {
    public:
        WallPotential() = delete;
        WallPotential(double origin, double normal);

        double getOrigin() const;
        void setOrigin(double origin);

        double getNormal() const;
        void setNormal(double normal);

    protected:
        double origin_;
        double normal_;
    };

}

#endif // FLYFT_WALL_POTENTIAL_H_
