#ifndef VIRIAL_EXPANSION_H_
#define VIRIAL_EXPANSION_H_

#include "flyft/functional.h"
#include "flyft/state.h"
#include "flyft/pair_map.h"

#include <memory>
#include <string>

namespace flyft
{

class VirialExpansion : public Functional
    {
    public:
        void compute(std::shared_ptr<State> state) override;

        const PairMap<double>& getCoefficients();
        void setCoefficients(const PairMap<double>& coeffs);

    private:
        PairMap<double> coeffs_;
    };

}

#endif // IDEAL_GAS_FUNCTIONAL_H_
