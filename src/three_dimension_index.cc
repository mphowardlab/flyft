#include "flyft/three_dimension_index.h"


namespace flyft
{
ThreeDimensionIndex::ThreeDimensionIndex(int ni , int nj, int nk)
    :ni_(ni), nj_(nj), nk_(nk)
    {        
    }

int ThreeDimensionIndex::operator()(int i, int j, int k) const
    {
    return (i*nj_+j)*nk_+k;
    }

int ThreeDimensionIndex::size() const
    {
    return ni_*nj_*nk_;
    }



}
