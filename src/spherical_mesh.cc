#include "flyft/spherical_mesh.h"

#include <cmath>

namespace flyft
{

std::shared_ptr<Mesh> SphericalMesh::slice(int start, int end) const
    {
    return std::shared_ptr<Mesh>(new SphericalMesh(end-start,step_,origin_+asLength(start)));
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
    if(idx == 0){
        return 0;
        }
    else{
        return (f_hi-f_lo)/(step_);
        }
    }   

}
