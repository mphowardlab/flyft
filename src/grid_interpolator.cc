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
        
    int nx, ny, nz;   
    indata >> x_ >> cutoff_ >> rho_ >> nx >> ny >> nz >> dx_ >> dy_ >> dz_;
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
    const double fx = x/dx_;
    const double fy = y/dy_;
    const double fz = z/dz_;
    const int bin_x = std::floor(fx); 
    const int bin_y = std::floor(fy); 
    const int bin_z = std::floor(fz); 
    const double xd = fx - bin_x;
    const double yd = fy - bin_y;
    const double zd = fz - bin_z;
    
    #ifndef NDEBUG
    const auto bounds = n_.getBounds();
    assert(bin_x >= 0 && bin_x < std::get<0>(bounds));
    assert(bin_y >= 0 && bin_y < std::get<1>(bounds));
    assert(bin_z >= 0 && bin_z < std::get<2>(bounds));
    #endif
    
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

std::tuple<double, double, double> GridInterpolator::getBounds() const
    {
    return std::make_tuple(x_, dx_, rho_);
    }
}
