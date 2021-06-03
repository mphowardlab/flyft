#ifndef BMCSL_HARD_SPHERE_FUNCTIONAL_H_
#define BMCSL_HARD_SPHERE_FUNCTIONAL_H_

#include "flyft/functional.h"
#include "flyft/state.h"
#include "flyft/type_map.h"

#include <memory>
#include <string>

namespace flyft
{

class BMCSLHardSphereFunctional : public Functional
    {
    public:
        void compute(std::shared_ptr<State> state) override;

        const TypeMap<double>& getDiameters();
        double getDiameter(const std::string& type) const;
        void setDiameters(const TypeMap<double>& diameters);
        void setDiameter(const std::string& type, double diameter);

    private:
        TypeMap<double> diameters_;
    };

}

#endif // BMCSL_HARD_SPHERE_FUNCTIONAL_H_
