#include "flyft/cartesian_mesh.h"

namespace flyft
{

CartesianMesh::CartesianMesh(double L, int shape)
    : CartesianMesh(L,shape,0)
    {
    }

CartesianMesh::CartesianMesh(double L,int shape, double origin)
    : Mesh(L,shape,origin), area_(1.0)
    {
    }

CartesianMesh::CartesianMesh(int shape, double step)
    : CartesianMesh(shape,step,0)
    {
    }

CartesianMesh::CartesianMesh(int shape, double step, double origin)
    : Mesh(shape,step,origin), area_(1.0)
    {
    }

double CartesianMesh::volume(int /*i*/) const
    {
    return area_*step_;
    }

}
