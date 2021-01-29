#ifndef FLYFT_FREE_ENERGY_FUNCTIONAL_H_
#define FLYFT_FREE_ENERGY_FUNCTIONAL_H_

#include "flyft/field.h"
#include "flyft/state.h"

#include <memory>
#include <vector>

namespace flyft
{

class FreeEnergyFunctional
    {
    public:
        FreeEnergyFunctional();
        virtual ~FreeEnergyFunctional();

        virtual void compute(std::shared_ptr<State> state) = 0;

        double getValue() const;

        const std::vector<std::shared_ptr<Field>>& getDerivatives();
        std::shared_ptr<Field> getDerivative(int idx);
        std::shared_ptr<const Field> getDerivative(int idx) const;

    protected:
        double value_;
        std::vector<std::shared_ptr<Field>> derivatives_;

        virtual void allocate(std::shared_ptr<State> state);
    };

}

#endif // FLYFT_FREE_ENERGY_FUNCTIONAL_H_
