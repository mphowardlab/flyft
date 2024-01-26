#ifndef FLYFT_THREE_DIMENSION_INDEX_H_
#define FLYFT_THREE_DIMENSION_INDEX_H_

#include<cmath>
#include<tuple>

namespace flyft
{
class ThreeDimensionalIndex
    {
    public:
        ThreeDimensionalIndex();
        ThreeDimensionalIndex(int nx, int ny, int nz);
        
        int operator()(int i, int j, int k) const;
        
        int size() const;
        
        std::tuple<int, int, int> getBounds() const;

        std::tuple<int, int, int> asTuple(int idx) const;
        
    private:
        int nx_;
        int ny_;
        int nz_;
    };
    
}

#endif // FLYFT_THREE_DIMENSION_INDEX_H_
