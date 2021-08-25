#include "flyft/virial_expansion.h"

#include <algorithm>

namespace flyft
{

VirialExpansion::VirialExpansion()
    {
    compute_depends_.add(&coeffs_);
    }

void VirialExpansion::_compute(std::shared_ptr<State> state, bool compute_value)
    {
    auto types = state->getTypes();
    const auto mesh = *state->getMesh()->local();

    // reset energy and chemical potentials to zero before accumulating
    value_ = 0.0;
    for (const auto& t : types)
        {
        std::fill(derivatives_(t)->view().begin(),derivatives_(t)->view().end(),0.);
        }

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
            shared(fi,di,fj,dj,compute_value) reduction(+:value_)
            #endif
            for (int idx=0; idx < mesh.shape(); ++idx)
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
                    value_ += mesh.step()*(factor*Bij*rhoi*rhoj);
                    }
                }
            }
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
