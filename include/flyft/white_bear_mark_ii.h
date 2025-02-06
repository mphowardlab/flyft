#ifndef FLYFT_WHITE_BEAR_MARK_II_H_
#define FLYFT_WHITE_BEAR_MARK_II_H_

#include "flyft/field.h"
#include "flyft/white_bear.h"

namespace flyft
    {

class WhiteBearMarkII : public WhiteBear
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

    } // namespace flyft

#endif // FLYFT_WHITE_BEAR_MARK_II_H_
