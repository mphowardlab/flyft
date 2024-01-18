#ifndef FLYFT_THREE_DIMENSION_INDEX_H_
#define FLYFT_THREE_DIMENSION_INDEX_H_

#include<cmath>

namespace flyft
{
class ThreeDimensionIndex
    {
    public:
        ThreeDimensionIndex(int ni, int nj, int nk);
        int size() const;
        int operator()(int i, int j, int k) const;
    protected:
        int ni_, nj_, nk_;
        int i, j, k;
    };
    
}

#endif // FLYFT_THREE_DIMENSION_INDEX_H_
