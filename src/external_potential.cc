#include "flyft/external_potential.h"

#include <cmath>

namespace flyft
{

bool ExternalPotential::setup(std::shared_ptr<State> state, bool compute_value)
    {
    bool compute = Functional::setup(state,compute_value);
    // only compute potentials if a compute is needed and one of the internal dependencies has changed
    if (compute)
        {
        compute_potentials_ = (!compute_token_ ||
                               token_.dirty() ||
                               token_ != compute_token_ ||
                               compute_depends_.changed());
        }
    else
        {
        compute_potentials_ = false;
        }
    return compute;
    }

void ExternalPotential::_compute(std::shared_ptr<State> state, bool compute_value)
    {
    // evaluate the external potential on the mesh, and store directly in derivative
    if (compute_potentials_)
        {
        for (const auto& t : state->getTypes())
            {
            potential(derivatives_(t),t,state);
            }
        compute_potentials_ = false;
        }

    // optionally accumulate value
    if (compute_value)
        {
        const auto mesh = *state->getMesh()->local();
        value_ = 0.0;
        for (const auto& t : state->getTypes())
            {
            auto f = state->getField(t)->const_view();
            auto d = derivatives_(t)->view();
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
                else if (compute_value)
                    {
                    value_ += mesh.step()*rho*V;
                    }
                }
            }
        value_ = state->getCommunicator()->sum(value_);
        }
    }
}
