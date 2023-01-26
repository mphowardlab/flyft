#include "flyft/spherical_mesh.h"

#include <cmath>

namespace flyft
{

std::shared_ptr<Mesh> SphericalMesh::slice(int start, int end) const
    {
    // TODO: fill this in
    return std::shared_ptr<Mesh>();
    }

double SphericalMesh::volume(int i) const
    {
    const double r_out = coordinate(i+1);
    const double r_in = coordinate(i);
    return (4.*M_PI/3.)*(r_out*r_out*r_out - r_in*r_in*r_in);
    }

}
