#ifndef FLYFT_WALL_POTENTIAL_H_
#define FLYFT_WALL_POTENTIAL_H_

#include "flyft/external_potential.h"
#include "flyft/parameter.h"

#include <memory>
#include <string>

namespace flyft
{

class WallPotential : public ExternalPotential
    {
    public:
        WallPotential() = delete;
        WallPotential(double origin, double normal);
        WallPotential(std::shared_ptr<DoubleParameter> origin, double normal);

        void compute(std::shared_ptr<State> state, bool compute_value) override;

        std::shared_ptr<DoubleParameter> getOrigin();
        std::shared_ptr<const DoubleParameter> getOrigin() const;
        void setOrigin(double origin);
        void setOrigin(std::shared_ptr<DoubleParameter> origin);

        double getNormal() const;
        void setNormal(double normal);

    protected:
        std::shared_ptr<DoubleParameter> origin_;
        double normal_;
    };

}

#endif // FLYFT_WALL_POTENTIAL_H_
