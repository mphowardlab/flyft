#ifndef FLYFT_GRID_INTERPOLATOR_H_
#define FLYFT_GRID_INTERPOLATOR_H_

#include "flyft/three_dimension_index.h"
#include <iostream>
#include <tuple>
#include <cmath>

namespace flyft
{

class GridInterpolator
    {  
    public:
        GridInterpolator(int ni, int nj, int nk, double dx, double dy, double dz, std::string s);
        ~GridInterpolator();
        
        double operator()(double x, double y, double z) const;   
        double* getData() const;
        
        std::tuple<int, int, int> getIndex();
        
        int ni, nj ,nk;
        
    protected:
        ThreeDimensionIndex n_;
        double* data_;
        double dx_;
        double dy_;
        double dz_;
    };
}

#endif // FLYFT_GRID_INTERPOLATOR_H_
