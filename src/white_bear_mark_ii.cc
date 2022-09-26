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
    const double n3tol = 1.e-2;
    f1 = -logvf;
    df1dn3 = vfinv;
    if (n3>n3tol)
        {
        const double vfinvsq = vfinv*vfinv;
        f2 = (5.*vfinv-n3sq*vfinv+2.*logvf)/(3.*n3);
        f4 = -1.*(n3*vfinvsq+(-3.+n3)*n3sq*vfinvsq+logvf)/(n3sq*36.*M_PI);
        df2dn3 = (-2.*vfinvsq*(n3-3.*n3sq+vf*vf*logvf))/(3.*n3sq);
        df4dn3 = (n3*(2.*vfinvsq*vfinv+(-5.*n3+n3sq)*vfinvsq)+2.*logvf)/(36.*n3sq*n3*M_PI);
        }
    else
        {
        f2 = 1.+n3+10.*n3sq/9.;
        f4 = (18.+n3*28.+39.*n3sq)/(432.*M_PI);
        df2dn3 = 1.+(20./9.)*n3+(7./2.)*n3sq;
        df4dn3 = (70.+195.*n3+378.*n3sq)/(1080.*M_PI); 
        }
    }
}
