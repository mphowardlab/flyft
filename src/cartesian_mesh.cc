#include "flyft/cartesian_mesh.h"

namespace flyft
    {

CartesianMesh::CartesianMesh(double lower_bound,
                             double upper_bound,
                             int shape,
                             BoundaryType lower_bc,
                             BoundaryType upper_bc,
                             double area)
    : Mesh(lower_bound, upper_bound, shape, lower_bc, upper_bc), area_(area)
    {
    }

std::shared_ptr<Mesh> CartesianMesh::clone() const
    {
    return std::make_shared<CartesianMesh>(*this);
    }

double CartesianMesh::area(int /*i*/) const
    {
    return area_;
    }

double CartesianMesh::volume() const
    {
    return area_ * L();
    }

double CartesianMesh::volume(int /*i*/) const
    {
    return area_ * step_;
    }

double CartesianMesh::gradient(int /*i*/, double f_lo, double f_hi) const
    {
    return (f_hi - f_lo) / step_;
    }
    } // namespace flyft
