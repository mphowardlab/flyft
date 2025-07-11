#include "flyft/cartesian_mesh.h"

#include <assert.h>

namespace flyft
    {

CartesianMesh::CartesianMesh(int num_orientation_dim,
                             int* shape,
                             double lower_bound,
                             double upper_bound,
                             BoundaryType lower_bc,
                             BoundaryType upper_bc,
                             double area)
    : Mesh(num_orientation_dim, shape, lower_bound, upper_bound, lower_bc, upper_bc), area_(area)
    {
    }

CartesianMesh::CartesianMesh(const CartesianMesh& other) : Mesh(other), area_(other.area_) {}

CartesianMesh& CartesianMesh::operator=(const CartesianMesh& other)
    {
    Mesh::operator=(other);
    if (this != &other)
        {
        area_ = other.area_;
        }
    return *this;
    }

CartesianMesh::CartesianMesh(const CartesianMesh&& other)
    : Mesh(other), area_(std::move(other.area_))
    {
    }

CartesianMesh& CartesianMesh::operator=(const CartesianMesh&& other)
    {
    Mesh::operator=(other);
    area_ = std::move(other.area_);
    return *this;
    }

#if 0
double CartesianMesh::area(int /*i*/) const
    {
    return area_;
    }
#endif

double CartesianMesh::cell_position_volume(const int* /*cell*/) const
    {
    assert(num_position_dim_ == 1);
    return area_ * step_[0];
    }

#if 0
double CartesianMesh::gradient(int /*i*/, double f_lo, double f_hi) const
    {
    return (f_hi - f_lo) / step_;
    }
#endif

std::shared_ptr<Mesh> CartesianMesh::clone() const
    {
    return std::make_shared<CartesianMesh>(*this);
    }

    } // namespace flyft
