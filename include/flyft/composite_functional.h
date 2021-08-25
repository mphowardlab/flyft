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
        CompositeFunctional();

        void requestDerivativeBuffer(const std::string& type, int buffer_request) override;
        int determineBufferShape(std::shared_ptr<State> state, const std::string& type) override;

        bool addObject(std::shared_ptr<Functional> object);
        bool removeObject(std::shared_ptr<Functional> object);
        void clearObjects();

    protected:
        bool setup(std::shared_ptr<State> state, bool compute_value) override;
        void _compute(std::shared_ptr<State> state, bool compute_value) override;
    };

}

#endif // FLYFT_COMPOSITE_FUNCTIONAL_H_
