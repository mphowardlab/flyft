#include "flyft/mesh.h"

#include <cmath>

namespace flyft
{

Mesh::Mesh(double L, int shape)
    : Mesh(L,shape,0.)
    {
    }

Mesh::Mesh(double L, int shape, double buffer_request)
    : L_(L)
    {
    step_ = L_/shape;
    const int buffer_shape = static_cast<int>(std::ceil(buffer_request/step_));
    buffer_ = buffer_shape*step_;
    layout_ = DataLayout(shape,buffer_shape);
    }

int Mesh::operator()(int i) const
    {
    // TODO: remove this wrapping, it's here for compatibility
    int idx = layout_(i);
    if (idx < 0)
        {
        idx += full_shape();
        }
    else if (idx >= full_shape())
        {
        idx -= full_shape();
        }
    return idx;
    }

double Mesh::coordinate(int i) const
    {
    // TODO: decide how this works, it currently operates on the index in [0,shape)
    return static_cast<double>(layout_(i)+0.5)*step_;
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
    return layout_.shape();
    }

int Mesh::buffer_shape() const
    {
    return layout_.buffer_shape();
    }

int Mesh::full_shape() const
    {
    return layout_.full_shape();
    }

const DataLayout& Mesh::layout() const
    {
    return layout_;
    }

double Mesh::step() const
    {
    return step_;
    }

}
