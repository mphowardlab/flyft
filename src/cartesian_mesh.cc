#include "flyft/cartesian_mesh.h"

double CartesianMesh::CartesianMesh(double L, int shape)
    : Mesh(L,shape), area_(1.0)
    {
    }
    
double CartesianMesh::volume(int i) const
    {
    return area_*step_;
    }
