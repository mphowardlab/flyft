#ifndef FLYFT_COMPOSITE_FLUX_H_
#define FLYFT_COMPOSITE_FLUX_H_

#include "flyft/composite_mixin.h"
#include "flyft/flux.h"
#include "flyft/grand_potential.h"
#include "flyft/state.h"

#include <memory>

namespace flyft
    {

class CompositeFlux : public Flux, public CompositeMixin<Flux>
    {
    public:
    using CompositeMixin<Flux>::CompositeMixin;
    void compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state) override;
    void requestFluxBuffer(const std::string& type, int buffer_request) override;
    };

    } // namespace flyft

#endif // FLYFT_COMPOSITE_FLUX_H_
