#include "flyft/virial_expansion.h"

#include <algorithm>

namespace flyft
{

void VirialExpansion::compute(std::shared_ptr<State> state)
    {
    allocate(state);

    auto types = state->getTypes();
    const auto mesh = *state->getMesh();

    // reset energy and chemical potentials to zero before accumulating
    value_ = 0.0;
    for (const auto& t : types)
        {
        std::fill(derivatives_.at(t)->begin(),derivatives_.at(t)->end(),0.);
        }

    for (auto it_i=types.cbegin(); it_i != types.cend(); ++it_i)
        {
        const auto i = *it_i;
        auto fi = state->getField(i)->cbegin();
        auto di = derivatives_.at(i)->begin();

        for (auto it_j=it_i; it_j != types.cend(); ++it_j)
            {
            const auto j = *it_j;
            auto fj = state->getField(j)->cbegin();
            auto dj = derivatives_.at(j)->begin();

            const double Bij = coeffs_(i,j);
            #ifdef FLYFT_OPENMP
            #pragma omp parallel for schedule(static) default(none) firstprivate(Bij,mesh) shared(fi,di,fj,dj) reduction(+:value_)
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
                value_ += mesh.step()*(factor*Bij*rhoi*rhoj);
                }
            }
        }
    }

const PairMap<double>& VirialExpansion::getCoefficients()
    {
    return coeffs_;
    }

void VirialExpansion::setCoefficients(const PairMap<double>& coeffs)
    {
    coeffs_ = PairMap<double>(coeffs);
    }

}
