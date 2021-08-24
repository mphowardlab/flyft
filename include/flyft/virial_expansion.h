#ifndef VIRIAL_EXPANSION_H_
#define VIRIAL_EXPANSION_H_

#include "flyft/functional.h"
#include "flyft/state.h"
#include "flyft/pair_map.h"

#include <memory>

namespace flyft
{

class VirialExpansion : public Functional
    {
    public:
        VirialExpansion();

        void compute(std::shared_ptr<State> state, bool compute_value) override;

        const PairMap<double>& getCoefficients();

    private:
        PairMap<double> coeffs_;
    };

}

#endif // VIRIAL_EXPANSION_H_
