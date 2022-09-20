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
    f1 = -logvf;
    f2 = vfinv;
    f4 = vfinv*vfinv/(24.*M_PI);
    df1dn3 = vfinv;
    df2dn3 = vfinv*vfinv;
    df4dn3 = 2.*f4*vfinv;
    }

}
