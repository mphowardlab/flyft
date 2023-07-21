#include "flyft/mesh.h"

#include <cmath>

namespace flyft
{

Mesh::Mesh(double lower, double upper, int shape, BoundaryType lower_bc, BoundaryType upper_bc)
    : lower_(lower), upper_(upper), L_(upper_-lower_), shape_(shape), step_(L_/shape_), lower_bc_(lower_bc), upper_bc_(upper_bc)
    {
    }

double Mesh::center(int i) const
    {
    return lower_+static_cast<double>(i+0.5)*step_;
    }

int Mesh::bin(double x) const
    {
    return static_cast<int>((x-lower_)/step_);
    }

double Mesh::lower_bound(int i) const
    {
    return lower_+static_cast<double>(i)*step_;
    }
    
double Mesh::upper_bound(int i) const
    {
    return lower_+static_cast<double>(i+1)*step_;
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

double Mesh::lower() const
    {
    return lower_;
    }

double Mesh::upper() const
    {
    return upper_;
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
    return (L_ == other.L_ && shape_ == other.shape_ && lower_ == other.lower_);
    }

bool Mesh::operator!=(const Mesh& other) const
    {
    return !(*this == other);
    }

BoundaryType Mesh::lower_boundary_condition() const
    {
    return lower_bc_;
    }  

BoundaryType Mesh::upper_boundary_condition() const
    {
    return upper_bc_;
    }


}
