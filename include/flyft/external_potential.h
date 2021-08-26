#ifndef FLYFT_EXTERNAL_POTENTIAL_H_
#define FLYFT_EXTERNAL_POTENTIAL_H_

#include "flyft/field.h"
#include "flyft/functional.h"
#include "flyft/state.h"

#include <memory>
#include <string>

namespace flyft
{

class ExternalPotential : public Functional
    {
    public:
        ExternalPotential();

        virtual void potential(std::shared_ptr<Field> V,
                               const std::string& type,
                               std::shared_ptr<State> state) = 0;

    protected:
        bool setup(std::shared_ptr<State> state, bool compute_value) override;
        void _compute(std::shared_ptr<State> state, bool compute_value) override;

    private:
        bool compute_potentials_;
        Mesh compute_potentials_mesh_;
    };

}

#endif // FLYFT_EXTERNAL_POTENTIAL_H_
