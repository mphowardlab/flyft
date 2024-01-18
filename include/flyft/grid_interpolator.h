#ifndef FLYFT_GRID_INTERPOLATOR_H_
#define FLYFT_GRID_INTERPOLATOR_H_

#include "flyft/three_dimension_index.h"

#include <cmath>

namespace flyft
{

class GridInterpolator
    {  
    public:
        GridInterpolator(ThreeDimensionIndex n, const double* data, double dx, double dy, double dz);
        double operator()(double x, double y, double z) const;   
    protected:
        ThreeDimensionIndex n_;
        const double* data_;
        double dx_;
        double dy_;
        double dz_;
    };
}

#endif // FLYFT_GRID_INTERPOLATOR_H_
