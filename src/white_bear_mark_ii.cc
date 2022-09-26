#include "flyft/white_bear_mark_ii.h"

namespace flyft
{

WhiteBearMarkII::WhiteBearMarkII()
    {
    }

void WhiteBearMarkII::computePrefactorFunctions(double& f1,
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
    const double n3sq = n3*n3;
    f1 = -logvf;
    df1dn3 = vfinv;
    if (n3<0.01)
    {
    f2 = 1+n3+10*n3sq/9;
    f4 = (18+n3*(28+39*n3))/(432.*M_PI);
    df2dn3 = 1+(20/9)*n3+(7/2)*n3sq;
    df4dn3 = (70+3.*n3*(65+126.*n3))/(1080*M_PI); 
    }
    else
    {
    f2 = (5.*n3-(n3sq)-2.*logvf*vf)*(vfinv/(3.*n3));
    f4 = -1*(n3+(-3+n3)*n3sq+vf*vf*logvf)*(vfinv*vfinv)*(1/(n3sq*36*M_PI));
    df2dn3 = (-2.)*(vfinv*vfinv)*(1/(3*n3sq))*(n3-3.*n3sq+vf*vf*logvf);
    df4dn3 = -1*(n3*(-2+(5.-n3)*vf*n3)-2.*vf*vf*vf*logvf)*(vfinv*vfinv*vfinv)*(1/(36.*n3sq*n3*M_PI));
    }
    }
}
