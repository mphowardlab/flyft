#include "flyft/grid_interpolator.h"

#include <cmath>
#include <cassert>
#include <fstream>

namespace flyft
{
GridInterpolator::GridInterpolator(const std::string& s)
    {
    std::ifstream indata(s);
    if(!indata)
        {
        throw std::runtime_error("File failed to open");
        }
    
    double x_low, x_high, y_low, y_high, z_low, z_high;
    int nx, ny, nz;
    
    indata >> x_low >> x_high >> y_low >> y_high >> z_low >> z_high >> nx >> ny >> nz;
    lower_bounds_ = std::make_tuple(x_low, y_low, z_low);
    upper_bounds_ = std::make_tuple(x_high, y_high, z_high);
    if(nx < 2 || ny < 2 || nz < 2)
        {
        throw std::invalid_argument("Number of points should be greater than 1");
        }

    dx_ = (x_high - x_low)/(nx - 1);
    dy_ = (y_high - y_low)/(ny - 1);
    dz_ = (z_high - z_low)/(nz - 1);
    
    n_ = ThreeDimensionalIndex(nx, ny, nz);
    if (n_.size() == 0)
        {
        throw std::runtime_error("Grid has zero size");
        }
        
    data_ = new double[n_.size()];  
    double value;
    
    int idx = 0;
    while (indata >> value)
        {
        data_[idx++] = value;
        }
        
    if (idx != n_.size() || !indata.eof())
        {
        throw std::runtime_error("File did not right number of elements");
        }
        
    indata.close();
    }

GridInterpolator::~GridInterpolator()
    {
    delete[] data_;
    }

double GridInterpolator::operator()(double x, double y, double z) const
    {
    const double fx = (x - std::get<0>(lower_bounds_))/dx_;
    const double fy = (y - std::get<1>(lower_bounds_))/dy_;
    const double fz = (z - std::get<2>(lower_bounds_))/dz_;
    
    const auto bounds = n_.getBounds();
    // x
    int bin_x = std::floor(fx);
    if(fx == std::get<0>(bounds) - 1 && x == std::get<0>(upper_bounds_))
       {
       /* for points exactly on the upper boundary, decrease by 1 to put it in
          bin below. this makes sure we only access bins within the bounds
          during interpolation
       */
       --bin_x;
       }
    // y
    int bin_y = std::floor(fy);
    if(fy == std::get<1>(bounds) - 1 && y == std::get<1>(upper_bounds_))
       {
       --bin_y;
       }
    // z
    int bin_z = std::floor(fz);
    if(fz == std::get<2>(bounds) - 1 && z == std::get<2>(upper_bounds_))
       {
       --bin_z;
       }

    const double xd = fx - bin_x;
    const double yd = fy - bin_y;
    const double zd = fz - bin_z;
    
    assert(bin_x >= 0 && bin_x < std::get<0>(bounds) - 1);
    assert(bin_y >= 0 && bin_y < std::get<1>(bounds) - 1);
    assert(bin_z >= 0 && bin_z < std::get<2>(bounds) - 1);
    
    const double c000 = data_[n_(bin_x,bin_y,bin_z)];
    const double c001 = data_[n_(bin_x,bin_y,bin_z+1)];
    const double c010 = data_[n_(bin_x,bin_y+1,bin_z)];
    const double c011 = data_[n_(bin_x,bin_y+1,bin_z+1)];
    const double c100 = data_[n_(bin_x+1,bin_y,bin_z)];
    const double c101 = data_[n_(bin_x+1,bin_y,bin_z+1)];
    const double c110 = data_[n_(bin_x+1,bin_y+1,bin_z)];
    const double c111 = data_[n_(bin_x+1,bin_y+1,bin_z+1)];
    
    const double c00 = c000*(1-xd)+c100*xd;
    const double c01 = c001*(1-xd)+c101*xd;
    const double c10 = c010*(1-xd)+c110*xd;
    const double c11 = c011*(1-xd)+c111*xd;
    
    const double c0 = c00*(1-yd)+c10*yd;
    const double c1 = c01*(1-yd)+c11*yd;
    
    return c0*(1-zd)+c1*zd;
    }

std::tuple<double, double, double> GridInterpolator::getLowerBounds() const
    {
    return lower_bounds_;
    }

std::tuple<double, double, double> GridInterpolator::getUpperBounds() const
    {
    return upper_bounds_;
    }
}
