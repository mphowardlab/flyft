#include "flyft/ideal_gas_functional.h"

namespace flyft
{

IdealGasFunctional::IdealGasFunctional()
    {
    compute_depends_.add(&volumes_);
    }

void IdealGasFunctional::compute(std::shared_ptr<State> state, bool compute_value)
    {
    bool needs_compute = setup(state,compute_value);
    if (!needs_compute)
        {
        return;
        }

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
        #pragma omp parallel for schedule(static) default(none) firstprivate(mesh,vol) \
        shared(f,d,compute_value) reduction(+:value_)
        #endif
        for (int idx=0; idx < mesh.shape(); ++idx)
            {
            const double rho = f(idx);
            double energy;
            if (rho > 0)
                {
                d(idx) = std::log(vol*rho);
                if (compute_value)
                    {
                    energy = mesh.step()*rho*(d(idx)-1.);
                    }
                else
                    {
                    energy = 0.;
                    }
                }
            else
                {
                d(idx) = -std::numeric_limits<double>::infinity();
                // no contribution to total in limit rho -> 0
                energy = 0.;
                }
            value_ += energy;
            }
        }

    if (compute_value)
        {
        value_ = state->getCommunicator()->sum(value_);
        }

    finalize(state,compute_value);
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
