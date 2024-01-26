#include "flyft/grid_interpolator.h"

#include <fstream> 
#include <cassert>

namespace flyft
{
GridInterpolator::GridInterpolator():
    n_(0,0,0)
    {
    }
    
GridInterpolator::GridInterpolator(const std::string& s)
    {
    int nx, ny, nz;
    double dx, dy, dz;
    
    std::ifstream indata(s);
    if(!indata)
        {
        throw std::runtime_error("File failed to open");
        }
        
    indata>>nx>>ny>>nz>>dx>>dy>>dz;
    n_ = ThreeDimensionIndex(nx,ny,nz);
    
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
    
    int bin_x = std::floor(x/dx_); 
    int bin_y = std::floor(y/dy_); 
    int bin_z = std::floor(z/dz_); 
    int ni, nj , nk;
    std::tie(ni, nj, nk) = n_.getBounds();
    assert(bin_x >= 0 && bin_x < ni);
    assert(bin_y >= 0 && bin_y < nj);
    assert(bin_z >= 0 && bin_z < nk);
    
    double xd = (x-(bin_x*dx_))/dx_;
    double yd = (y-(bin_y*dy_))/dy_;
    double zd = (z-(bin_z*dz_))/dz_;
    
    double c000 = data_[n_(bin_x,bin_y,bin_z)];
    double c001 = data_[n_(bin_x,bin_y,bin_z+1)];
    double c010 = data_[n_(bin_x,bin_y+1,bin_z)];
    double c011 = data_[n_(bin_x,bin_y+1,bin_z+1)];
    double c100 = data_[n_(bin_x+1,bin_y,bin_z)];
    double c101 = data_[n_(bin_x+1,bin_y,bin_z+1)];
    double c110 = data_[n_(bin_x+1,bin_y+1,bin_z)];
    double c111 = data_[n_(bin_x+1,bin_y+1,bin_z+1)];
    
    double c00 = c000*(1-xd)+c100*xd;
    double c01 = c001*(1-xd)+c101*xd;
    double c10 = c010*(1-xd)+c110*xd;
    double c11 = c011*(1-xd)+c111*xd;
    
    double c0 = c00*(1-yd)+c10*yd;
    double c1 = c01*(1-yd)+c11*yd;
    
    return c0*(1-zd)+c1*zd;
    }

double* GridInterpolator::getData() const
    {
    return data_;
    }


}
