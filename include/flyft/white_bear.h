#ifndef FLYFT_WHITE_BEAR_H_
#define FLYFT_WHITE_BEAR_H_

#include "flyft/field.h"
#include "flyft/rosenfeld_fmt.h"

namespace flyft
{

class WhiteBear : public RosenfeldFMT
    {
    protected:
        void computePrefactorFunctions(double& f1,
                                       double& f2,
                                       double& f4,
                                       double& df1dn3,
                                       double& df2dn3,
                                       double& df4dn3,
                                       double n3) const override;
    };

}

#endif // FLYFT_WHITE_BEAR_H_
