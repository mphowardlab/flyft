#include "flyft/external_potential.h"

#include <cmath>

namespace flyft
{

void ExternalPotential::compute(std::shared_ptr<State> state)
    {
    allocate(state);

    // compute derivatives and accumulate energy
    const auto mesh = *state->getMesh();
    value_ = 0.0;
    for (const auto& t : state->getTypes())
        {
        // evaluate the external potential on the mesh, and store directly in derivative
        potential(derivatives_.at(t),t,state);

        // compute the total potential by integration
        auto f = state->getField(t)->cbegin();
        auto d = derivatives_.at(t)->begin();
        for (int idx=0; idx < mesh.shape(); ++idx)
            {
            const double V = d(idx);
            const double rho = f(idx);
            if (std::isinf(V))
                {
                if (rho > 0)
                    {
                    // density present and infinite potential, stop summing
                    value_ = V;
                    break;
                    }
                else
                    {
                    // no contribution, ignore
                    }
                }
            else
                {
                value_ += mesh.step()*rho*V;
                }
            }
        }
    }
}
