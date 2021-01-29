#include "flyft/mesh.h"

namespace flyft
{

Mesh::Mesh(double L, int shape)
    : L_(L), shape_(shape)
    {
    step_ = L_/shape_;
    }

double Mesh::coordinate(int i) const
    {
    return (static_cast<double>(i)+0.5)*step_;
    }

int Mesh::bin(double x) const
    {
    return static_cast<int>((x-0.5*L_)/step_);
    }

int Mesh::shape() const
    {
    return shape_;
    }

double Mesh::step() const
    {
    return step_;
    }

}
