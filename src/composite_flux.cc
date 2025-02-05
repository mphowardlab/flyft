#include "flyft/composite_flux.h"

#include <algorithm>

namespace flyft
    {

void CompositeFlux::compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state)
    {
    setup(grand, state);

    // initialize to zeros
    for (const auto& t : state->getTypes())
        {
        auto j = fluxes_(t)->full_view();
        std::fill(j.begin(), j.end(), 0.);
        }

    // combine
    for (const auto& o : objects_)
        {
        o->compute(grand, state);
        for (const auto& t : state->getTypes())
            {
            auto j = fluxes_(t)->full_view();
            auto jo = o->getFlux(t)->const_full_view();
            std::transform(j.begin(), j.end(), jo.begin(), j.begin(), std::plus<>());
            }
        }
    }

void CompositeFlux::requestFluxBuffer(const std::string& type, int buffer_request)
    {
    Flux::requestFluxBuffer(type, buffer_request);
    for (auto& o : objects_)
        {
        o->requestFluxBuffer(type, buffer_request);
        }
    }

    } // namespace flyft
