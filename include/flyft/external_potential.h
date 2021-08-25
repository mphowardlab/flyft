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
        virtual void potential(std::shared_ptr<Field> V,
                               const std::string& type,
                               std::shared_ptr<State> state) = 0;

    protected:
        void _compute(std::shared_ptr<State> state, bool compute_value) override;
    };

}

#endif // FLYFT_EXTERNAL_POTENTIAL_H_
