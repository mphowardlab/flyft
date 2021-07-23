#include "flyft/ideal_gas_functional.h"

namespace flyft
{

void IdealGasFunctional::compute(std::shared_ptr<State> state)
    {
    allocate(state);

    // compute derivatives and accumulate energy
    const auto mesh = *state->getMesh();
    value_ = 0.0;
    for (const auto& t : state->getTypes())
        {
        const auto vol = volumes_.at(t);

        // compute the total potential by integration
        auto f = state->getField(t)->cbegin();
        auto d = derivatives_.at(t)->begin();
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(mesh,vol) shared(f,d) reduction(+:value_)
        #endif
        for (int idx=0; idx < mesh.shape(); ++idx)
            {
            const int self = mesh(idx);
            const double rho = f[self];
            double energy;
            if (rho > 0)
                {
                d[self] = std::log(vol*rho);
                energy = mesh.step()*rho*(d[self]-1.);
                }
            else
                {
                d[self] = -std::numeric_limits<double>::infinity();
                // no contribution to total in limit rho -> 0
                energy = 0.0;
                }

            value_ += energy;
            }
        }
    }

const TypeMap<double>& IdealGasFunctional::getVolumes()
    {
    return volumes_;
    }

double IdealGasFunctional::getVolume(const std::string& type) const
    {
    return volumes_.at(type);
    }

void IdealGasFunctional::setVolumes(const TypeMap<double>& volumes)
    {
    volumes_ = TypeMap<double>(volumes);
    }

void IdealGasFunctional::setVolume(const std::string& type, double volume)
    {
    if (volume > 0.)
        {
        volumes_[type] = volume;
        }
    else
        {
        // error: invalid volume
        }
    }

}
