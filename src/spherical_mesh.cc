#include "flyft/spherical_mesh.h"

#include <cmath>
#include <exception>

namespace flyft
    {

std::shared_ptr<Mesh> SphericalMesh::clone() const
    {
    return std::make_shared<SphericalMesh>(*this);
    }

#if 0
double SphericalMesh::area(int i) const
    {
    const double r = lower_bound(i);
    return 4. * M_PI * r * r;
    }
#endif

double SphericalMesh::cell_position_volume(const int* cell) const
    {
    const double r_in = cell_lower_bound(0, cell[0]);
    const double r_out = cell_upper_bound(0, cell[0]);
    return (4. * M_PI / 3.) * (r_out * r_out * r_out - r_in * r_in * r_in);
    }

#if 0
double SphericalMesh::gradient(int /*i*/, double f_lo, double f_hi) const
    {
    return (f_hi - f_lo) / (step_);
    }
#endif

bool SphericalMesh::validate_boundary_conditions() const
    {
    bool invalid = !Mesh::validate_boundary_conditions();

    invalid |= (lower_bc_ == BoundaryType::periodic || upper_bc_ == BoundaryType::periodic);
    invalid |= (upper_bc_ == BoundaryType::reflect);

    return !invalid;
    }
    } // namespace flyft
