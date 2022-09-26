#include "flyft/white_bear.h"

namespace flyft
{

WhiteBear::WhiteBear()
    {
    }

void WhiteBear::computePrefactorFunctions(double& f1,
                                          double& f2,
                                          double& f4,
                                          double& df1dn3,
                                          double& df2dn3,
                                          double& df4dn3,
                                          double n3) const
    {
    // precompute the "void fraction" vf, which is only a function of n3
    const double vf = 1.-n3;
    if (vf < 0)
        {
        // local void fraction unphysical
        }
    const double logvf = std::log(vf);
    const double vfinv = 1./vf;
    f1 = -logvf;
    f2 = vfinv;
    df1dn3 = vfinv;
    const double vfinvsq = vfinv*vfinv;
    df2dn3 = vfinvsq;
    const double n3tol = 1.e-2;
    if (n3 > n3tol)
        {
        const double n3sq = n3*n3;
        f4 = (n3*vfinvsq+logvf)/(36.*M_PI*n3sq);
        df4dn3 = (-n3*(2.+(-5.*n3+n3sq))*(vfinvsq*vfinv)+2.*logvf)/(36.*M_PI*n3sq*n3);
        }
    else
        {
        f4 = (18.+n3*(32.+45.*n3))/(432.*M_PI);
        df4dn3 = (80.+9.*n3*(25.+48.*n3))/(1080.*M_PI) ;
        }
    }

}
