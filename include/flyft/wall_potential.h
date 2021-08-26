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

        std::shared_ptr<DoubleParameter> getOrigin();
        std::shared_ptr<const DoubleParameter> getOrigin() const;
        void setOrigin(double origin);
        void setOrigin(std::shared_ptr<DoubleParameter> origin);

        std::shared_ptr<DoubleParameter> getNormal();
        std::shared_ptr<const DoubleParameter> getNormal() const;
        void setNormal(double normal);
        void setNormal(std::shared_ptr<DoubleParameter> normal);

    protected:
        std::shared_ptr<DoubleParameter> origin_;
        std::shared_ptr<DoubleParameter> normal_;

        bool setup(std::shared_ptr<State> state, bool compute_value) override;

        double computeNormal(std::shared_ptr<State> state);
    };

}

#endif // FLYFT_WALL_POTENTIAL_H_
