#include "flyft/ideal_gas_functional.h"

namespace flyft
{

IdealGasFunctional::IdealGasFunctional()
    {
    compute_depends_.add(&volumes_);
    }

static void computeFunctional(int idx,
                              Field::View& d,
                              double& value,
                              const Field::ConstantView& f,
                              const double vol,
                              const Mesh* mesh,
                              bool compute_value)
    {
    const double rho = f(idx);
    double energy;
    if (rho > 0)
        {
        d(idx) = std::log(vol*rho);
        if (compute_value)
            {
            energy = mesh->step()*rho*(d(idx)-1.);
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
    value += energy;
    }

void IdealGasFunctional::_compute(std::shared_ptr<State> state, bool compute_value)
    {
    value_ = 0.0;
    for (const auto& t : state->getTypes())
        {
        auto deriv = derivatives_(t);

        auto d = deriv->view();
        auto f = state->getField(t)->const_view();
        const auto vol = volumes_(t);
        const auto mesh = state->getMesh()->local().get();

        // compute edges of each derivative first and put in flight
        const auto deriv_buffer = deriv->buffer_shape();
        for (int idx=0; idx < deriv_buffer; ++idx)
            {
            computeFunctional(idx,d,value_,f,vol,mesh,compute_value);
            }
        for (int idx=mesh->shape()-deriv_buffer; idx < mesh->shape(); ++idx)
            {
            computeFunctional(idx,d,value_,f,vol,mesh,compute_value);
            }
        state->getMesh()->startSync(deriv);

        // compute on interior points
        #ifdef FLYFT_OPENMP
        #pragma omp parallel for schedule(static) default(none) firstprivate(mesh,vol) \
        shared(f,d,compute_value,deriv_buffer) reduction(+:value_)
        #endif
        for (int idx=deriv_buffer; idx < mesh->shape()-deriv_buffer; ++idx)
            {
            computeFunctional(idx,d,value_,f,vol,mesh,compute_value);
            }
        }

    // finalize all derivative communications
    for (const auto& t : state->getTypes())
        {
        state->getMesh()->endSync(derivatives_(t));
        }

    // reduce value across ranks
    if (compute_value)
        {
        value_ = state->getCommunicator()->sum(value_);
        }
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
