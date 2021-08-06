#ifndef FLYFT_COMPOSITE_FUNCTIONAL_H_
#define FLYFT_COMPOSITE_FUNCTIONAL_H_

#include "flyft/composite_mixin.h"
#include "flyft/functional.h"
#include "flyft/state.h"

#include <memory>

namespace flyft
{

class CompositeFunctional : public Functional, public CompositeMixin<Functional>
    {
    public:
        using CompositeMixin<Functional>::CompositeMixin;
        void compute(std::shared_ptr<State> state) override;
        void requestDerivativeBuffer(const std::string& type, int buffer_request) override;
        int determineBufferShape(std::shared_ptr<State> state, const std::string& type) override;
    };

}

#endif // FLYFT_COMPOSITE_FUNCTIONAL_H_
