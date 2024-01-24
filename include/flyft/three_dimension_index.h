#ifndef FLYFT_THREE_DIMENSION_INDEX_H_
#define FLYFT_THREE_DIMENSION_INDEX_H_

#include<cmath>
#include<tuple>

namespace flyft
{
class ThreeDimensionIndex
    {
    public:
        ThreeDimensionIndex(int ni, int nj, int nk);
        int size() const;
        
        int operator()(int i, int j, int k) const;
        
        std::tuple<int, int, int> getBounds() const;

        std::tuple<int, int, int> backmapping(int k) const;
    protected:
        int index_;
        int ni_, nj_, nk_;
    };
    
}

#endif // FLYFT_THREE_DIMENSION_INDEX_H_
