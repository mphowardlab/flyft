#ifndef FLYFT_EXTERNAL_POTENTIAL_H_
#define FLYFT_EXTERNAL_POTENTIAL_H_

#include "flyft/field.h"
#include "flyft/functional.h"
#include "flyft/state.h"

#include <cmath>
#include <memory>
#include <string>

namespace flyft
{

class ExternalPotential : public Functional
    {
    public:
        ExternalPotential();

    protected:
        bool setup(std::shared_ptr<State> state, bool compute_value) override;
        void _compute(std::shared_ptr<State> state, bool compute_value) override;
        virtual void computePotentials(std::shared_ptr<State> state) = 0;

    private:
        bool compute_potentials_;
        Mesh compute_potentials_mesh_;
    };

}

#endif // FLYFT_EXTERNAL_POTENTIAL_H_
