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
        GridInterpolator(std::string s);
        GridInterpolator(std::string s, int ni, int nj, int nk);
        GridInterpolator(std::string s,int ni, int nj, int nk, double dx, double dy, double dz);
        ~GridInterpolator();
        
        double operator()(double x, double y, double z) const;   
        double* getData() const;
        
        std::tuple<int, int, int> getIndex();
        
        
    protected:
        ThreeDimensionIndex n_;
        double* data_;
        double dx_;
        double dy_;
        double dz_;
    };
}

#endif // FLYFT_GRID_INTERPOLATOR_H_
