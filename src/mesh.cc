#include "flyft/mesh.h"

#include <cmath>

namespace flyft
{

Mesh::Mesh(double L, int shape)
    : L_(L), shape_(shape), step_(L_/shape_)
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

int Mesh::shape() const
    {
    return shape_;
    }

double Mesh::step() const
    {
    return step_;
    }

int Mesh::asShape(double dx) const
    {
    return static_cast<int>(std::ceil(dx/step_));
    }

double Mesh::asLength(int shape) const
    {
    return shape*step_;
    }

}
