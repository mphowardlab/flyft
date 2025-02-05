#ifndef FLYFT_WALL_POTENTIAL_H_
#define FLYFT_WALL_POTENTIAL_H_

#include "flyft/parameter.h"
#include "flyft/templated_external_potential.h"

#include <memory>
#include <string>

namespace flyft
    {

class WallPotentialFunction
    {
    public:
    WallPotentialFunction(double origin, double normal) : origin_(origin), normal_(normal) {}

    protected:
    double origin_;
    double normal_;

    double distance(double x) const
        {
        return normal_ * (x - origin_);
        }
    };

template<class PotentialFunction>
class WallPotential : public TemplatedExternalPotential<PotentialFunction>
    {
    public:
    WallPotential() = delete;
    WallPotential(double origin, double normal)
        : WallPotential(std::make_shared<ConstantDoubleParameter>(origin), normal)
        {
        }
    WallPotential(std::shared_ptr<DoubleParameter> origin, double normal)
        {
        setOrigin(origin);
        setNormal(normal);
        }

    std::shared_ptr<DoubleParameter> getOrigin()
        {
        return this->origin_;
        }
    std::shared_ptr<const DoubleParameter> getOrigin() const
        {
        return this->origin_;
        }
    void setOrigin(double origin)
        {
        setOrigin(std::make_shared<ConstantDoubleParameter>(origin));
        }
    void setOrigin(std::shared_ptr<DoubleParameter> origin)
        {
        if (this->origin_)
            {
            this->compute_depends_.remove(this->origin_->id());
            }
        this->origin_ = origin;
        this->compute_depends_.add(this->origin_.get());
        }

    std::shared_ptr<DoubleParameter> getNormal()
        {
        return this->normal_;
        }
    std::shared_ptr<const DoubleParameter> getNormal() const
        {
        return this->normal_;
        }
    void setNormal(double normal)
        {
        setNormal(std::make_shared<ConstantDoubleParameter>(normal));
        }
    void setNormal(std::shared_ptr<DoubleParameter> normal)
        {
        if (this->normal_)
            {
            this->compute_depends_.remove(this->normal_->id());
            }
        this->normal_ = normal;
        this->compute_depends_.add(this->normal_.get());
        }

    protected:
    std::shared_ptr<DoubleParameter> origin_;
    std::shared_ptr<DoubleParameter> normal_;

    bool setup(std::shared_ptr<State> state, bool compute_value) override
        {
        this->origin_->evaluate(state);
        this->normal_->evaluate(state);
        return TemplatedExternalPotential<PotentialFunction>::setup(state, compute_value);
        }

    double computeNormal(std::shared_ptr<State> state)
        {
        double n = this->normal_->evaluate(state);
        if (n > 0)
            {
            n = 1.0;
            }
        else if (n < 0)
            {
            n = -1.0;
            }
        else
            {
            throw std::runtime_error("Normal must be nonzero");
            }
        return n;
        }
    };

    } // namespace flyft

#endif // FLYFT_WALL_POTENTIAL_H_
