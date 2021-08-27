#include "flyft/external_potential.h"

namespace flyft
{

ExternalPotential::ExternalPotential()
    : compute_potentials_mesh_(0.,1)
    {
    }

bool ExternalPotential::setup(std::shared_ptr<State> state, bool compute_value)
    {
    bool compute = Functional::setup(state,compute_value);
    // only compute potentials if a compute is needed and one of the internal dependencies has changed
    if (compute)
        {
        compute_potentials_ = (!compute_token_ ||
                                compute_depends_.changed() ||
                                compute_potentials_mesh_ != *state->getMesh()->local());
        }
    else
        {
        compute_potentials_ = false;
        }
    return compute;
    }


void ExternalPotential::_compute(std::shared_ptr<State> state, bool compute_value)
    {
    const auto mesh = *state->getMesh()->local();

    // evaluate the external potential on the mesh, and store directly in derivative
    if (compute_potentials_)
        {
        computePotentials(state);
        compute_potentials_ = false;
        compute_potentials_mesh_ = mesh;
        }

    // optionally accumulate value
    if (compute_value)
        {
        value_ = 0.0;
        for (const auto& t : state->getTypes())
            {
            auto f = state->getField(t)->const_view();
            auto d = derivatives_(t)->const_view();
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
