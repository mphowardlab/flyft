#include "flyft/cartesian_mesh.h"


double CartesianMesh::defaultArea()
{
    area_= 1.;
    return area_;
}

double CartesianMesh::volume(int i) const
    {
    return area_*step_;
    }
