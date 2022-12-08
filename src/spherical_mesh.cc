#include "flyft/spherical_mesh.h"

#include <cmath>

namespace flyft
{

SphericalMesh::volume(int i) const
    {
    outer_vol = (i+step_)*(i+step_)*(i+step_);
    vol = i*i*i;
    return 4/3*M_PI*(outer_vol-vol);
    }
