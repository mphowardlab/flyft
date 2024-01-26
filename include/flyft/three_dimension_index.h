#ifndef FLYFT_THREE_DIMENSION_INDEX_H_
#define FLYFT_THREE_DIMENSION_INDEX_H_

#include<cmath>
#include<tuple>

namespace flyft
{
class ThreeDimensionIndex
    {
    public:
        ThreeDimensionIndex();
        ThreeDimensionIndex(int nx, int ny, int nz);
        int size() const;
        
        int operator()(int i, int j, int k) const;
        
        std::tuple<int, int, int> getBounds() const;

        std::tuple<int, int, int> backmapping(int k) const;
        
    private:
        int nx_, ny_, nz_;
    };
    
}

#endif // FLYFT_THREE_DIMENSION_INDEX_H_
