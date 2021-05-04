#ifndef FLYFT_COMPOSITE_EXTERNAL_POTENTIAL_H_
#define FLYFT_COMPOSITE_EXTERNAL_POTENTIAL_H_

#include "flyft/composite_mixin.h"
#include "flyft/field.h"
#include "flyft/external_potential.h"
#include "flyft/state.h"

#include <memory>
#include <string>

namespace flyft
{

class CompositeExternalPotential : public ExternalPotential, public CompositeMixin<ExternalPotential>
    {
    public:
        using CompositeMixin<ExternalPotential>::CompositeMixin;
        void potential(std::shared_ptr<Field> V, const std::string& type, std::shared_ptr<State> state) override;

    private:
        std::shared_ptr<Field> Vtmp_;
    };

}

#endif // FLYFT_COMPOSITE_EXTERNAL_POTENTIAL_H_
