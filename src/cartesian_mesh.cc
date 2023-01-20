#include "flyft/cartesian_mesh.h"

double CartesianMesh::CartesianMesh(double L, int shape)
    : Mesh(L,shape), area_(1.0)
    {
    }

double CartesianMesh::CartesianMesh(double L,int shape, double origin)
    : Mesh(L,shape,origin)
    {
    }

double CartesianMesh::CartesianMesh(int shape,double step)
    : Mesh(shape,step,0)
    {
    }

double CartesianMesh::CartesianMesh(int shape, double step, double origin)
    : Mesh(shape,step,origin)
    {
    }

double CartesianMesh::volume(int i) const
    {
    return area_*step_;
    }
