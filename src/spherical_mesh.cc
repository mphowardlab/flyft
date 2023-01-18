#include "flyft/spherical_mesh.h"

#include <cmath>

namespace flyft
{

SphericalMesh::volume(int i) const
    {
    out = coordinate(i+1)*coordinate(i+1);
    in = coordinate(i)*coordinate(i);
    return (4/3)*M_PI*(coordinate(i+1)-coordinate(i))*(out+coordinate(i)*coordinate(i+1)+in);
    }
