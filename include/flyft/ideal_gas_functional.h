#ifndef IDEAL_GAS_FUNCTIONAL_H_
#define IDEAL_GAS_FUNCTIONAL_H_

#include "flyft/free_energy_functional.h"
#include "flyft/state.h"

#include <memory>

namespace flyft
{

class IdealGasFunctional : public FreeEnergyFunctional
    {
    public:
        void compute(std::shared_ptr<State> state) override;
    };

}

#endif // IDEAL_GAS_FUNCTIONAL_H_
