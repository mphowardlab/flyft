#ifndef FLYFT_FREE_ENERGY_FUNCTIONAL_H_
#define FLYFT_FREE_ENERGY_FUNCTIONAL_H_

#include "flyft/field.h"
#include "flyft/state.h"
#include "flyft/type_map.h"

#include <memory>
#include <string>
#include <vector>

namespace flyft
{

class FreeEnergyFunctional
    {
    public:
        FreeEnergyFunctional();
        virtual ~FreeEnergyFunctional();

        // noncopyable / nonmovable
        FreeEnergyFunctional(const FreeEnergyFunctional&) = delete;
        FreeEnergyFunctional(FreeEnergyFunctional&&) = delete;
        FreeEnergyFunctional& operator=(const FreeEnergyFunctional&) = delete;
        FreeEnergyFunctional& operator=(FreeEnergyFunctional&&) = delete;

        virtual void compute(std::shared_ptr<State> state) = 0;

        double getValue() const;

        const TypeMap<std::shared_ptr<Field>>& getDerivatives();
        std::shared_ptr<Field> getDerivative(const std::string& type);
        std::shared_ptr<const Field> getDerivative(const std::string& type) const;

    protected:
        double value_;
        TypeMap<std::shared_ptr<Field>> derivatives_;

        virtual void allocate(std::shared_ptr<State> state);
    };

}

#endif // FLYFT_FREE_ENERGY_FUNCTIONAL_H_
