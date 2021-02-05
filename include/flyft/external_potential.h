#ifndef FLYFT_EXTERNAL_POTENTIAL_H_
#define FLYFT_EXTERNAL_POTENTIAL_H_

#include "flyft/field.h"
#include "flyft/free_energy_functional.h"
#include "flyft/mesh.h"
#include "flyft/state.h"

namespace flyft
{

class ExternalPotential : public FreeEnergyFunctional
    {
    public:
        void compute(std::shared_ptr<State> state) override;

    protected:
        virtual void potential(std::shared_ptr<Field> V,
                               int idx,
                               std::shared_ptr<State> state) = 0;

    };

}

#endif // FLYFT_EXTERNAL_POTENTIAL_H_
