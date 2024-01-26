#include "flyft/three_dimension_index.h"


namespace flyft
{
ThreeDimensionIndex::ThreeDimensionIndex(int nx , int ny, int nz)
    :nx_(nx), ny_(ny), nz_(nz)
    {        
    }

int ThreeDimensionIndex::operator()(int i, int j, int k) const
    {
    return (i*ny_+j)*nz_+k;
    }

int ThreeDimensionIndex::size() const
    {
    return nx_*ny_*nz_;
    }

std::tuple<int, int, int>ThreeDimensionIndex::getBounds() const
    {
    return std::make_tuple(nx_, ny_, nz_);
    }

std::tuple<int, int, int> ThreeDimensionIndex::backmapping(int k) const
    {
    const int z = k % (nz_);
    const int y = (k / nz_) % ny_;
    const int x =  k / (ny_ * nz_);
    return std::make_tuple(x,y,z);
    }

}
