#ifndef FLYFT_GRID_INTERPOLATOR_H_
#define FLYFT_GRID_INTERPOLATOR_H_

#include "flyft/three_dimensional_index.h"

#include <string>

namespace flyft
{

class GridInterpolator
    {  
    public:
        GridInterpolator(const std::string& s);
        ~GridInterpolator();
        
        double operator()(double x, double y, double z) const;   
        
    private:
        ThreeDimensionalIndex n_;
        double* data_;
        double dx_;
        double dy_;
        double dz_;
    };
}

#endif // FLYFT_GRID_INTERPOLATOR_H_
