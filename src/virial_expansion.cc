#include "flyft/virial_expansion.h"

#include <algorithm>

namespace flyft
{

VirialExpansion::VirialExpansion()
    {
    compute_depends_.add(&coeffs_);
    }

static void computeFunctional(int idx,
                              Field::View& di,
                              Field::View& dj,
                              double& value,
                              const Field::ConstantView& fi,
                              const Field::ConstantView& fj,
                              double Bij,
                              const Mesh* mesh,
                              bool compute_value)
    {
    const double rhoi = fi(idx);
    const double rhoj = fj(idx);
    di(idx) += 2*Bij*rhoj;
    // since we are only taking half the double sum, need to add to other type / total
    double factor = 1.0;
    if (dj != di)
        {
        dj(idx) += 2*Bij*rhoi;
        factor = 2.0;
        }
    if (compute_value)
        {
        value += mesh->volume(idx)*(factor*Bij*rhoi*rhoj);
        }
    }

void VirialExpansion::_compute(std::shared_ptr<State> state, bool compute_value)
    {
    auto types = state->getTypes();
    const auto mesh = state->getMesh()->local().get();

    // reset energy and chemical potentials to zero before accumulating
    value_ = 0.0;
    int max_deriv_buffer = 0;
    for (const auto& t : types)
        {
        std::fill(derivatives_(t)->view().begin(),derivatives_(t)->view().end(),0.);
        max_deriv_buffer = std::max(max_deriv_buffer,derivatives_(t)->buffer_shape());
        }

    // begin calculation on edges and send
    for (auto it_i=types.cbegin(); it_i != types.cend(); ++it_i)
        {
        const auto i = *it_i;
        auto fi = state->getField(i)->const_view();
        auto di = derivatives_(i)->view();

        for (auto it_j=it_i; it_j != types.cend(); ++it_j)
            {
            const auto j = *it_j;
            auto fj = state->getField(j)->const_view();
            auto dj = derivatives_(j)->view();

            const double Bij = coeffs_(i,j);
            for (int idx=0; idx < max_deriv_buffer; ++idx)
                {
                computeFunctional(idx,di,dj,value_,fi,fj,Bij,mesh,compute_value);
                }
            for (int idx=mesh->shape()-max_deriv_buffer; idx < mesh->shape(); ++idx)
                {
                computeFunctional(idx,di,dj,value_,fi,fj,Bij,mesh,compute_value);
                }
            }

        // all the contributions to this type are done, so start syncing
        state->getMesh()->startSync(derivatives_(i));
        }

    // calculate on interior points
    for (auto it_i=types.cbegin(); it_i != types.cend(); ++it_i)
        {
        const auto i = *it_i;
        auto fi = state->getField(i)->const_view();
        auto di = derivatives_(i)->view();

        for (auto it_j=it_i; it_j != types.cend(); ++it_j)
            {
            const auto j = *it_j;
            auto fj = state->getField(j)->const_view();
            auto dj = derivatives_(j)->view();

            const double Bij = coeffs_(i,j);
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(Bij,mesh) \
            shared(fi,di,fj,dj,compute_value,max_deriv_buffer) reduction(+:value_)
            #endif
            for (int idx=max_deriv_buffer; idx < mesh->shape()-max_deriv_buffer; ++idx)
                {
                computeFunctional(idx,di,dj,value_,fi,fj,Bij,mesh,compute_value);
                }
            }
        }

    // finalize communication
    for (const auto& t : types)
        {
        state->getMesh()->endSync(derivatives_(t));
        }

    if (compute_value)
        {
        value_ = state->getCommunicator()->sum(value_);
        }
    }

PairMap<double>& VirialExpansion::getCoefficients()
    {
    return coeffs_;
    }

}
