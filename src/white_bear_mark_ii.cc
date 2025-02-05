#include "flyft/white_bear_mark_ii.h"

namespace flyft
    {

void WhiteBearMarkII::computePrefactorFunctions(double& f1,
                                                double& f2,
                                                double& f4,
                                                double& df1dn3,
                                                double& df2dn3,
                                                double& df4dn3,
                                                double n3) const
    {
    // precompute the "void fraction" vf, which is only a function of n3
    const double vf = 1. - n3;
    if (vf < 0)
        {
        // local void fraction unphysical
        }
    const double logvf = std::log(vf);
    const double vfinv = 1. / vf;

    f1 = -logvf;
    df1dn3 = vfinv;
    const double n3tol = 1.e-2;
    if (n3 > n3tol)
        {
        const double vfinvsq = vfinv * vfinv;
        const double f4_denom = 36. * M_PI * n3 * n3;
        f2 = ((5. - n3) * vfinv + 2. * logvf / n3) / 3.;
        f4 = -(n3 * (1. + n3 * (-3. + n3)) * vfinvsq + logvf) / f4_denom;
        df2dn3 = -2. * ((1. - 3. * n3) * vfinvsq + logvf / n3) / (3. * n3);
        df4dn3 = ((2. * vfinv + n3 * (-5. + n3)) * vfinvsq + 2. * logvf / n3) / f4_denom;
        }
    else
        {
        // second-order Taylor series to deal with numerics at small n3
        const double n3sq = n3 * n3;
        f2 = 1. + n3 + 10. * n3sq / 9.;
        f4 = (18. + 28. * n3 + 39. * n3sq) / (432. * M_PI);
        df2dn3 = 1. + (20. / 9.) * n3 + (7. / 2.) * n3sq;
        df4dn3 = (70. + 195. * n3 + 378. * n3sq) / (1080. * M_PI);
        }
    }
    } // namespace flyft
