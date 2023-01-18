#include "flyft/cartesian_mesh.h"


double CartesianMesh::volume(int i) const
    {
    return area_*step_;
    }
