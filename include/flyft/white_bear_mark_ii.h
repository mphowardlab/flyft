#ifndef FLYFT_WHITE_BEAR_MARK_II_H_
#define FLYFT_WHITE_BEAR_MARK_II_H_

#include "flyft/field.h"
#include "flyft/white_bear.h"

namespace flyft
{
    
class WhiteBearMarkII : public WhiteBear
    {
    public:
        WhiteBearMarkII();

    protected:
        void computePhiAndDerivatives(int idx,
                                      Field::View& phi,
                                      Field::View& dphi_dn0,
                                      Field::View& dphi_dn1,
                                      Field::View& dphi_dn2,
                                      Field::View& dphi_dn3,
                                      Field::View& dphi_dnv1,
                                      Field::View& dphi_dnv2,
                                      const Field::ConstantView& n0,
                                      const Field::ConstantView& n1,
                                      const Field::ConstantView& n2,
                                      const Field::ConstantView& n3,
                                      const Field::ConstantView& nv1,
                                      const Field::ConstantView& nv2,
                                      bool compute_value) override;
    };

}

#endif // FLYFT_WHITE_BEAR_MARK_II_H_
