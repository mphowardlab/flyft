#ifndef BOUBLIK_HARD_SPHERE_FUNCTIONAL_H_
#define BOUBLIK_HARD_SPHERE_FUNCTIONAL_H_

#include "flyft/functional.h"
#include "flyft/state.h"
#include "flyft/type_map.h"

#include <memory>

namespace flyft
{

class BoublikHardSphereFunctional : public Functional
    {
    public:
        BoublikHardSphereFunctional();
        void compute(std::shared_ptr<State> state) override;

        TypeMap<double>& getDiameters();
        const TypeMap<double>& getDiameters() const;

    private:
        TypeMap<double> diameters_;
    };

}

#endif // BOUBLIK_HARD_SPHERE_FUNCTIONAL_H_
