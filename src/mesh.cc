#include "flyft/mesh.h"

#include <cmath>

namespace flyft
{

Mesh::Mesh(double L, int shape)
    : Mesh(L,shape,0)
    {
    }

Mesh::Mesh(double L, int shape, double origin)
    : L_(L), shape_(shape), step_(L_/shape_), origin_(origin)
    {
    }

Mesh::Mesh(int shape, double step)
    : Mesh(shape,step,0)
    {
    }

Mesh::Mesh(int shape, double step, double origin)
    : L_(shape*step), shape_(shape), step_(step), origin_(origin)
    {
    }

double Mesh::coordinate(int i) const
    {
    return origin_+static_cast<double>(i+0.5)*step_;
    }

int Mesh::bin(double x) const
    {
    return static_cast<int>((x-origin_)/step_);
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

double Mesh::origin() const
    {
    return origin_;
    }

int Mesh::asShape(double dx) const
    {
    return static_cast<int>(std::ceil(dx/step_));
    }

double Mesh::asLength(int shape) const
    {
    return shape*step_;
    }
    
double Mesh::integrateSurface(int idx, double j_lo, double j_hi) const
    {
    return area(idx)*j_lo - area(idx+1)*j_hi;
    }
    
double Mesh::integrateSurface(int idx, const DataView<double>& j) const
    {
    return integrateSurface(idx, j(idx), j(idx+1));
    }

double Mesh::integrateVolume(int idx, double f) const
    {
    return volume(idx)*f;
    }
double Mesh::integrateVolume(int idx, const DataView<double>& f) const
    {
    return integrateVolume(idx, f(idx));
    }

bool Mesh::operator==(const Mesh& other) const
    {
    return (L_ == other.L_ && shape_ == other.shape_ && origin_ == other.origin_);
    }

bool Mesh::operator!=(const Mesh& other) const
    {
    return !(*this == other);
    }

}
