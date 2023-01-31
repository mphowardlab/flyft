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

std::shared_ptr<Mesh> CartesianMesh::slice(int start, int end) const
    {
    return std::shared_ptr<Mesh>(new CartesianMesh(end-start,step_,origin_+asLength(start)));
    }

double CartesianMesh::area(int /*i*/) const
    {
        return area_;
    }

double CartesianMesh::volume(int /*i*/) const
    {
    return area_*step_;
    }

}
