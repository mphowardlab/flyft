#include "flyft/mesh.h"
#include <iostream>

#include <cmath>

namespace flyft
{

Mesh::Mesh(double L, int shape, BoundaryType lower_bc, BoundaryType upper_bc)
    : Mesh(L,shape,0, lower_bc, upper_bc )
    {
    }

Mesh::Mesh(double L, int shape, double origin,BoundaryType lower_bc, BoundaryType upper_bc)
    : L_(L), shape_(shape), step_(L_/shape_), origin_(origin), lower_bc_(lower_bc), upper_bc_(upper_bc)
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

double Mesh::center(int i) const
    {
    return origin_+static_cast<double>(i+0.5)*step_;
    }

int Mesh::bin(double x) const
    {
    return static_cast<int>((x-origin_)/step_);
    }

double Mesh::lower_bound(int i) const
    {
    return origin_+static_cast<double>(i)*step_;
    }
    
double Mesh::upper_bound(int i) const
    {
    return origin_+static_cast<double>(i+1)*step_;
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

double Mesh::integrateSurface(int idx,const DataView<double>& j) const
    {
    return integrateSurface(idx,j(idx),j(idx+1));
    }

double Mesh::integrateSurface(int idx,const DataView<const double>& j) const
    {
    return integrateSurface(idx,j(idx),j(idx+1));
    }

double Mesh::integrateVolume(int idx, double f) const
    {
    return volume(idx)*f;
    }

double Mesh::integrateVolume(int idx, const DataView<double>& f) const
    {
    return integrateVolume(idx, f(idx));
    }

double Mesh::integrateVolume(int idx, const DataView<const double>& f) const
    {
    return integrateVolume(idx, f(idx));
    }

double Mesh::interpolate(double x, const DataView<double>& f) const
    {
    return interpolate(x,f);
    }

double Mesh::interpolate(double x, const DataView<const double>& f) const
    {
    const auto idx = bin(x);
    const auto x_c = center(idx);

    double x_0, x_1, f_0, f_1;
    if (x < x_c && lower_bound(idx) != 0)// TODO: Fix this in communicator. ITS A HACK!
        {
        x_0 = center(idx - 1);
        x_1 = x_c;
        f_0 = f(idx - 1);
        f_1 = f(idx);
        }
    else
        {
        x_0 = x_c;
        x_1 = center(idx + 1);
        f_0 = f(idx);
        f_1 = f(idx + 1);
        }

    return f_0 + (x - x_0) * (f_1 - f_0) / (x_1 - x_0);
    }

double Mesh::gradient(int idx, const DataView<double>& f) const
    {
    return gradient(idx, f(idx-1), f(idx));
    }

double Mesh::gradient(int idx, const DataView<const double>& f) const
    {
    return gradient(idx, f(idx-1), f(idx));
    }

bool Mesh::operator==(const Mesh& other) const
    {
    return (L_ == other.L_ && shape_ == other.shape_ && origin_ == other.origin_);
    }

bool Mesh::operator!=(const Mesh& other) const
    {
    return !(*this == other);
    }

BoundaryType Mesh::getlower_boundary_type() const
    {
    return lower_bc_;
    }  

BoundaryType Mesh::getupper_boundary_type() const
    {
    return upper_bc_;
    }


}
