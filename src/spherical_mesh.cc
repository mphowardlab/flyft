#include "flyft/spherical_mesh.h"

#include <cmath>

namespace flyft
{
SphericalMesh::SphericalMesh(double R, int shape)
    : SphericalMesh(R,shape,0)
    {
    }

SphericalMesh::SphericalMesh(double R,int shape, double origin)
    : Mesh(R,shape,origin)
    {
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

}
