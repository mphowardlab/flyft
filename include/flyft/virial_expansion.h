#ifndef VIRIAL_EXPANSION_H_
#define VIRIAL_EXPANSION_H_

#include "flyft/functional.h"
#include "flyft/pair_map.h"
#include "flyft/state.h"

#include <memory>

namespace flyft
    {

class VirialExpansion : public Functional
    {
    public:
    VirialExpansion();

    PairMap<double>& getCoefficients();

    private:
    void _compute(std::shared_ptr<State> state, bool compute_value) override;

    private:
    PairMap<double> coeffs_;
    };

    } // namespace flyft

#endif // VIRIAL_EXPANSION_H_
