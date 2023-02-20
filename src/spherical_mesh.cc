#include "flyft/spherical_mesh.h"

#include <cmath>

namespace flyft
{
SphericalMesh::SphericalMesh(double L, int shape)
    : SphericalMesh(L,shape,0)
    {
    }

SphericalMesh::SphericalMesh(double L,int shape, double origin)
    : Mesh(L,shape,origin)
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
    const double r = coordinate(i);
    return 4.*M_PI*r*r;
    }


double SphericalMesh::volume(int i) const
    {
    const double r_out = coordinate(i+1);
    const double r_in = coordinate(i);
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
