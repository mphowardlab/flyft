#include "flyft/grid_interpolator.h"


namespace flyft
{
GridInterpolator::GridInterpolator(ThreeDimensionIndex n, const double* data, double dx, double dy, double dz):
    n_(n), data_(data), dx_(dx), dy_(dy), dz_(dz)
    {        
    }
}
