#ifndef IDEAL_GAS_FUNCTIONAL_H_
#define IDEAL_GAS_FUNCTIONAL_H_

#include "flyft/functional.h"
#include "flyft/state.h"
#include "flyft/type_map.h"

#include <memory>
#include <string>

namespace flyft
{

class IdealGasFunctional : public Functional
    {
    public:
        void compute(std::shared_ptr<State> state, bool compute_value) override;

        TypeMap<double>& getVolumes();
        const TypeMap<double>& getVolumes() const;

    private:
        TypeMap<double> volumes_;
    };

}

#endif // IDEAL_GAS_FUNCTIONAL_H_
