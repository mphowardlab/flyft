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

        TypeMap<double>& getDiameters();
        const TypeMap<double>& getDiameters() const;

    protected:
        void _compute(std::shared_ptr<State> state, bool compute_value) override;

    private:
        TypeMap<double> diameters_;
    };

}

#endif // BOUBLIK_HARD_SPHERE_FUNCTIONAL_H_
