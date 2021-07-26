#include "flyft/mesh.h"

#include <cmath>

namespace flyft
{

Mesh::Mesh(double L, int shape)
    : L_(L), shape_(shape), step_(L_/shape)
    {
    setBuffer(0);
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

int Mesh::shape() const
    {
    return shape_;
    }

int Mesh::buffer_shape() const
    {
    return buffer_shape_;
    }

DataLayout Mesh::layout() const
    {
    return DataLayout(shape_,buffer_shape_);
    }

double Mesh::step() const
    {
    return step_;
    }

void Mesh::setBuffer(double buffer_request)
    {
    buffer_shape_ = static_cast<int>(std::ceil(buffer_request/step_));
    buffer_ = buffer_shape_*step_;
    }

}
