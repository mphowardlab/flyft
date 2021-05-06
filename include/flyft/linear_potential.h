#ifndef FLYFT_LINEAR_POTENTIAL_H_
#define FLYFT_LINEAR_POTENTIAL_H_

#include "flyft/external_potential.h"
#include "flyft/field.h"
#include "flyft/state.h"
#include "flyft/type_map.h"

#include <memory>
#include <string>

namespace flyft
{

class LinearPotential : public ExternalPotential
    {
    public:
        void potential(std::shared_ptr<Field> V,
                       const std::string& type,
                       std::shared_ptr<State> state) override;

        const TypeMap<double>& getXs();
        double getX(const std::string& type) const;
        void setXs(const TypeMap<double>& xs);
        void setX(const std::string& type, double x);

        const TypeMap<double>& getYs();
        double getY(const std::string& type) const;
        void setYs(const TypeMap<double>& ys);
        void setY(const std::string& type, double y);

        const TypeMap<double>& getSlopes();
        double getSlope(const std::string& type) const;
        void setSlopes(const TypeMap<double>& slopes);
        void setSlope(const std::string& type, double slope);

    protected:
        TypeMap<double> xs_;
        TypeMap<double> ys_;
        TypeMap<double> slopes_;
    };

}

#endif // FLYFT_LINEAR_POTENTIAL_H_
