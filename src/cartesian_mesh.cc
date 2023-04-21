#include "flyft/cartesian_mesh.h"

namespace flyft
{

CartesianMesh::CartesianMesh(double L, int shape, double area)
    : CartesianMesh(L,shape,area,0)
    {
    }

CartesianMesh::CartesianMesh(double L,int shape, double area, double origin)
    : Mesh(L,shape,origin), area_(area)
    {
    }

CartesianMesh::CartesianMesh(int shape, double step, double area)
    : CartesianMesh(shape,step,area,0)
    {
    }

CartesianMesh::CartesianMesh(int shape, double step,double area, double origin)
    : Mesh(shape,step,origin),area_(area)
    {
    }

std::shared_ptr<Mesh> CartesianMesh::slice(int start, int end) const
    {
    return std::shared_ptr<Mesh>(new CartesianMesh(end-start,step_,area_,lower_bound(start)));
    }

double CartesianMesh::area(int /*i*/) const
    {
    return area_;
    }

double CartesianMesh::volume() const
    {
    return area_*L_;
    }

double CartesianMesh::volume(int /*i*/) const
    {
    return area_*step_;
    }

double CartesianMesh::gradient(int /*i*/, double f_lo, double f_hi) const
    {
    return (f_hi-f_lo)/step_; 
    }

}
