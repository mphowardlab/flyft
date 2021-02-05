#include "flyft/ideal_gas_functional.h"

namespace flyft
{

void IdealGasFunctional::compute(std::shared_ptr<State> state)
    {
    allocate(state);

    // compute derivatives and accumulate energy
    auto mesh = state->getMesh();
    value_ = 0.0;
    for (int i=0; i < state->getNumFields(); ++i)
        {
        const auto vol = state->getIdealVolume(i);

        // compute the total potential by integration
        auto f = state->getField(i)->data();
        auto d = derivatives_[i]->data();
        for (int idx=0; idx < mesh->shape(); ++idx)
            {
            const double rho = f[idx];
            if (rho > 0)
                {
                d[idx] = std::log(vol*rho);
                value_ += mesh->step()*rho*(d[idx]-1.);
                }
            else
                {
                d[idx] = -std::numeric_limits<double>::infinity();
                }
            }
        }
    }

}
