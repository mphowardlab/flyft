#include "flyft/white_bear.h"

namespace flyft
    {

void WhiteBear::computePrefactorFunctions(double& f1,
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
        // local void frac
        }
    const double logvf = std::log(vf);
    const double vfinv = 1. / vf;
    const double vfinvsq = vfinv * vfinv;

    f1 = -logvf;
    f2 = vfinv;
    df1dn3 = vfinv;
    df2dn3 = vfinvsq;
    const double n3tol = 1.e-2;
    if (n3 > n3tol)
        {
        const double f4_denom = 36. * M_PI * n3 * n3;
        f4 = (n3 * vfinvsq + logvf) / f4_denom;
        df4dn3 = -((2. + n3 * (-5. + n3)) * (vfinvsq * vfinv) + 2. * logvf / n3) / f4_denom;
        }
    else
        {
        // second-order Taylor series to deal with numerics at small n3
        const double n3sq = n3 * n3;
        f4 = (18. + 32. * n3 + 45. * n3sq) / (432. * M_PI);
        df4dn3 = (80. + 225. * n3 + 432. * n3sq) / (1080. * M_PI);
        }
    }

    } // namespace flyft
