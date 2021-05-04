#include "flyft/composite_flux.h"

#include <algorithm>

namespace flyft
{

void CompositeFlux::compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state)
    {
    allocate(state);
    auto mesh = state->getMesh();

    // initialize to zeros
    for (const auto& t : state->getTypes())
        {
        auto j = fluxes_.at(t)->data();
        std::fill(j,j+mesh->shape(),0.);
        }

    // combine
    for (const auto& o : objects_)
        {
        o->compute(grand, state);
        for (const auto& t : state->getTypes())
            {
            auto j = fluxes_.at(t)->data();
            auto jo = o->getFlux(t)->data();
            for (int idx=0; idx < mesh->shape(); ++idx)
                {
                j[idx] += jo[idx];
                }
            }
        }
    }

}
