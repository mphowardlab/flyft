#include "flyft/three_dimensional_index.h"

namespace flyft
{
ThreeDimensionalIndex::ThreeDimensionalIndex()
    : nx_(0), ny_(0), nz_(0)
    {}

ThreeDimensionalIndex::ThreeDimensionalIndex(int nx , int ny, int nz)
    : nx_(nx), ny_(ny), nz_(nz)
    {}

int ThreeDimensionalIndex::operator()(int i, int j, int k) const
    {
    return (i*ny_+j)*nz_+k;
    }

int ThreeDimensionalIndex::size() const
    {
    return nx_*ny_*nz_;
    }

std::tuple<int, int, int>ThreeDimensionalIndex::getBounds() const
    {
    return std::make_tuple(nx_, ny_, nz_);
    }

std::tuple<int, int, int> ThreeDimensionalIndex::asTuple(int idx) const
    {
    const int z = idx % nz_;
    const int y = (idx / nz_) % ny_;
    const int x =  idx / (ny_ * nz_);
    return std::make_tuple(x, y, z);
    }

}
