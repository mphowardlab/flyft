#include "flyft/spherical_mesh.h"

#include <cmath>

namespace flyft
{

SphericalMesh::volume(int i) const
    {
    r_out = coordinate(i+1);
    r_in = coordinate(i);
    return (4.*M_PI/3.)*(r_out*r_out*r_out - r_in*r_in*r_in);
    }
