#include "flyft/cartesian_mesh.h"

#include <cmath>

double CartesianMesh::volume(int i) const
    {
    return area_*step_;
    }
