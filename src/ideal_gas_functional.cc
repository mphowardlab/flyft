#include "flyft/ideal_gas_functional.h"

namespace flyft
{

void IdealGasFunctional::compute(std::shared_ptr<State> state)
    {
    setup(state);

    // compute derivatives and accumulate energy
    const auto mesh = *state->getMesh()->local();
    value_ = 0.0;
    for (const auto& t : state->getTypes())
        {
        const auto vol = volumes_(t);

        // compute the total potential by integration
        auto f = state->getField(t)->const_view();
        auto d = derivatives_(t)->view();
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(mesh,vol) shared(f,d) reduction(+:value_)
        #endif
        for (int idx=0; idx < mesh.shape(); ++idx)
            {
            const double rho = f(idx);
            double energy;
            if (rho > 0)
                {
                d(idx) = std::log(vol*rho);
                energy = mesh.step()*rho*(d(idx)-1.);
                }
            else
                {
                d(idx) = -std::numeric_limits<double>::infinity();
                // no contribution to total in limit rho -> 0
                energy = 0.0;
                }
            value_ += energy;
            }
        }

    value_ = state->getCommunicator()->sum(value_);
    }

TypeMap<double>& IdealGasFunctional::getVolumes()
    {
    return volumes_;
    }

const TypeMap<double>& IdealGasFunctional::getVolumes() const
    {
    return volumes_;
    }

}
