#include "flyft/reciprocal_mesh.h"

#include <cmath>

namespace flyft
{

ReciprocalMesh::ReciprocalMesh(double L, int shape)
    : L_(L), shape_(shape)
    {
    step_ = (2.*M_PI)/L_;
    mid_bin_ = (shape_ % 2 == 0) ? shape_/2 : (shape_+1)/2;
    }

ReciprocalMesh::ReciprocalMesh(const Mesh& mesh)
    : ReciprocalMesh(mesh.L(), mesh.shape())
    {}

double ReciprocalMesh::coordinate(int i) const
    {
    if (i < mid_bin_)
        {
        return static_cast<double>(i)*step_;
        }
    else
        {
        return -static_cast<double>(shape_-i)*step_;
        }
    }

double ReciprocalMesh::L() const
    {
    return L_;
    }

int ReciprocalMesh::shape() const
    {
    return shape_;
    }

double ReciprocalMesh::step() const
    {
    return step_;
    }

}
