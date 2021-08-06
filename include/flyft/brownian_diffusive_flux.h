#ifndef FLYFT_BROWNIAN_DIFFUSIVE_FLUX_H_
#define FLYFT_BROWNIAN_DIFFUSIVE_FLUX_H_

#include "flyft/diffusive_flux.h"
#include "flyft/grand_potential.h"
#include "flyft/state.h"

#include <memory>

namespace flyft
{

class BrownianDiffusiveFlux : public DiffusiveFlux
    {
    public:
        void compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state) override;
        int determineBufferShape(std::shared_ptr<State> state, const std::string& type) override;
    };

}

#endif // FLYFT_BROWNIAN_DIFFUSIVE_FLUX_H_
