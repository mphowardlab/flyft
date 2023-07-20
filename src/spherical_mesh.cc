#include "flyft/spherical_mesh.h"

#include <cmath>
#include <iostream>

namespace flyft
{

// SphericalMesh::SphericalMesh(double lower, double upper, int shape, BoundaryType upper_bc)
//     : SphericalMesh(lower, upper,shape, BoundaryType::reflect, upper_bc)
//     {
//     }

SphericalMesh::SphericalMesh(double lower, double upper,int shape, BoundaryType lower_bc, BoundaryType upper_bc)
    : Mesh(lower, upper, shape, lower_bc, upper_bc)
    {
    }


std::shared_ptr<Mesh> SphericalMesh::slice(int start, int end) const
    {
    return std::shared_ptr<Mesh>(new SphericalMesh(lower_bound(start),end,end-start,lower_bc_, upper_bc_));
    }

double SphericalMesh::area(int i) const
    {
    const double r = lower_bound(i);
    return 4.*M_PI*r*r;
    }

double SphericalMesh::volume() const
    {
    const double rlo = lower_bound(0);
    const double rhi = upper_bound(shape_-1);
    return (4.*M_PI/3.)*(rhi*rhi*rhi - rlo*rlo*rlo);
    }
    
double SphericalMesh::volume(int i) const
    {
    const double r_out = upper_bound(i);
    const double r_in = lower_bound(i);
    return (4.*M_PI/3.)*(r_out*r_out*r_out - r_in*r_in*r_in);
    }

double SphericalMesh::gradient(int idx, double f_lo, double f_hi) const
    {
    if (idx == 0 && origin_ == 0.){
        return 0;
        }
    else{
        return (f_hi-f_lo)/(step());
        }
    }   
void SphericalMesh::validateBoundaryCondition()
    {
    if (lower_bc_ == BoundaryType::periodic){
        throw std::invalid_argument("Lower boundary type not valid for spherical geometry");
        }
    else if (upper_bc_ == BoundaryType::periodic){
        throw std::invalid_argument("Upper boundary type not valid for spherical geometry");
        }
    else{
        return;
        }
    }
}
