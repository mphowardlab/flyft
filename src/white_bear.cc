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
    df2dn3 = vfinv*vfinv;
    const double n3sq = n3*n3;
    if (n3<0.01)
    {
    f4 = (18+n3*(32+45*n3))/(432*M_PI);
    df4dn3 = (70+3*n3*(65+126*n3))/(1080*M_PI) ;
    }
    else
    {
    f4 = (n3+vf*vf*logvf)*(vfinv*vfinv)*(1/(n3sq*36*M_PI));
    df4dn3 = -1*(n3*(-2+(5-n3)*vf*n3)+2*vf*vf*vf*logvf)*(vfinv*vfinv*vfinv)*(1/(n3sq*n3*M_PI));
    }
    }

}
