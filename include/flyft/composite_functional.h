#ifndef FLYFT_COMPOSITE_FUNCTIONAL_H_
#define FLYFT_COMPOSITE_FUNCTIONAL_H_

#include "flyft/free_energy_functional.h"
#include "flyft/state.h"

#include <memory>
#include <vector>

namespace flyft
{

class CompositeFunctional : public FreeEnergyFunctional
    {
    public:
        CompositeFunctional();
        CompositeFunctional(std::shared_ptr<FreeEnergyFunctional> functional);
        CompositeFunctional(const std::vector<std::shared_ptr<FreeEnergyFunctional>>& functionals);

        void compute(std::shared_ptr<State> state) override;

        void addFunctional(std::shared_ptr<FreeEnergyFunctional> functional);
        void removeFunctional(std::shared_ptr<FreeEnergyFunctional> functional);
        void clearFunctionals();
        const std::vector<std::shared_ptr<FreeEnergyFunctional>>& getFunctionals();

    private:
        std::vector<std::shared_ptr<FreeEnergyFunctional>> functionals_;
    };

}

#endif // FLYFT_COMPOSITE_FUNCTIONAL_H_
