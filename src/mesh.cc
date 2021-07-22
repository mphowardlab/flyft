#include "flyft/mesh.h"

#include <cmath>

namespace flyft
{

Mesh::Mesh(double L, int shape)
    : Mesh(L,shape,0.)
    {
    }

Mesh::Mesh(double L, int shape, double buffer_request)
    : L_(L), shape_(shape)
    {
    step_ = L_/shape_;
    buffer_shape_ = static_cast<int>(std::ceil(buffer_request/step_));
    buffer_ = buffer_shape_*step_;
    }

double Mesh::coordinate(int i) const
    {
    // TODO: decide how this works, it currently operates on the index in [0,shape)
    return (static_cast<double>(i)+0.5)*step_;
    }

int Mesh::bin(double x) const
    {
    return static_cast<int>(x/step_);
    }

double Mesh::L() const
    {
    return L_;
    }

int Mesh::shape() const
    {
    return shape_;
    }

int Mesh::buffer_shape() const
    {
    return buffer_shape_;
    }

int Mesh::buffered_shape() const
    {
    return shape_ + 2*buffer_shape_;
    }

double Mesh::step() const
    {
    return step_;
    }

}
