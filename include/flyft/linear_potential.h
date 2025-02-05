#ifndef FLYFT_LINEAR_POTENTIAL_H_
#define FLYFT_LINEAR_POTENTIAL_H_

#include "flyft/field.h"
#include "flyft/state.h"
#include "flyft/templated_external_potential.h"
#include "flyft/type_map.h"

#include <memory>
#include <string>

namespace flyft
    {

class LinearPotentialFunction
    {
    public:
    LinearPotentialFunction(double x0, double y0, double slope) : x0_(x0), y0_(y0), slope_(slope) {}

    double operator()(double x) const
        {
        return y0_ + slope_ * (x - x0_);
        }

    private:
    double x0_;
    double y0_;
    double slope_;
    };

class LinearPotential : public TemplatedExternalPotential<LinearPotentialFunction>
    {
    public:
    LinearPotential();

    TypeMap<double>& getXs();
    const TypeMap<double>& getXs() const;

    TypeMap<double>& getYs();
    const TypeMap<double>& getYs() const;

    TypeMap<double>& getSlopes();
    const TypeMap<double>& getSlopes() const;

    protected:
    Function makePotentialFunction(std::shared_ptr<State> state, const std::string& type) override;

    protected:
    TypeMap<double> xs_;
    TypeMap<double> ys_;
    TypeMap<double> slopes_;
    };

    } // namespace flyft

#endif // FLYFT_LINEAR_POTENTIAL_H_
