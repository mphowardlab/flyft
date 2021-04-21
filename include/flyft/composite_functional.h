#ifndef FLYFT_COMPOSITE_FUNCTIONAL_H_
#define FLYFT_COMPOSITE_FUNCTIONAL_H_

#include "flyft/functional.h"
#include "flyft/state.h"

#include <memory>
#include <vector>

namespace flyft
{

class CompositeFunctional : public Functional
    {
    public:
        CompositeFunctional();
        CompositeFunctional(std::shared_ptr<Functional> functional);
        CompositeFunctional(const std::vector<std::shared_ptr<Functional>>& functionals);

        void compute(std::shared_ptr<State> state) override;

        void addFunctional(std::shared_ptr<Functional> functional);
        void removeFunctional(std::shared_ptr<Functional> functional);
        void clearFunctionals();
        const std::vector<std::shared_ptr<Functional>>& getFunctionals();

    private:
        std::vector<std::shared_ptr<Functional>> functionals_;
    };

}

#endif // FLYFT_COMPOSITE_FUNCTIONAL_H_
