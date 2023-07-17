#include "flyft/spherical_mesh.h"

#include <cmath>
#include <iostream>

namespace flyft
{

SphericalMesh::SphericalMesh(double R, int shape, BoundaryType upper_bc)
    : SphericalMesh(R,shape,0, BoundaryType::reflect, upper_bc)
    {
    }

SphericalMesh::SphericalMesh(double R,int shape, double origin, BoundaryType lower_bc, BoundaryType upper_bc)
    : Mesh(R,shape,origin, lower_bc, upper_bc)
    {
    if(upper_bc==BoundaryType::periodic)
        {
        std::cout<<"Boundary type is not valid";
        }
    }

SphericalMesh::SphericalMesh(int shape, double step)
    :SphericalMesh(shape,step,0)
    {
    }

SphericalMesh::SphericalMesh(int shape, double step, double origin)
    : Mesh(shape, step, origin)
    {
    }

std::shared_ptr<Mesh> SphericalMesh::slice(int start, int end) const
    {
    return std::shared_ptr<Mesh>(new SphericalMesh(end-start,step_,lower_bound(start)));
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
        return (f_hi-f_lo)/(step_);
        }
    }   

BoundaryType SphericalMesh::setlowerbound() const
    {
    return lower_bc_;
    }  

BoundaryType SphericalMesh::setupperbound() const
    {
    return upper_bc_;
    }

}
