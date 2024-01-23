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

std::tuple<int, int, int>ThreeDimensionIndex::getIndex() const
    {
    return std::make_tuple(ni_, nj_, nk_);
    }

std::tuple<int, int, int> ThreeDimensionIndex::backmapping(int k) const
    {
    int z = k%(nk_);
    int y = ((k-z)/nk_)%nj_;
    int x =  (((k-z)/nk_)-y)/nj_;
    return std::make_tuple(x,y,z);
    }

}
