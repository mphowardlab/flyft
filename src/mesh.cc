#include "flyft/mesh.h"

#include <cmath>

namespace flyft
{

Mesh::Mesh(double L, int shape)
    : L_(L), buffer_(0), step_(L_/shape), layout_(shape,0)
    {
    }

double Mesh::coordinate(int i) const
    {
    return static_cast<double>(i+0.5)*step_;
    }

int Mesh::bin(double x) const
    {
    return static_cast<int>(x/step_);
    }

double Mesh::L() const
    {
    return L_;
    }

double Mesh::buffer() const
    {
    return buffer_;
    }

double Mesh::step() const
    {
    return step_;
    }

const DataLayout& Mesh::layout() const
    {
    return layout_;
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

void Mesh::setBuffer(double buffer_request)
    {
    const int buffer_shape = static_cast<int>(std::ceil(buffer_request/step_));
    buffer_ = buffer_shape*step_;
    layout_ = DataLayout(layout_.shape(),buffer_shape);
    }

}
