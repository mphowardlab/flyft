#include "flyft/spherical_mesh.h"

#include <cmath>
#include <exception>

namespace flyft
{
SphericalMesh::SphericalMesh(double lower_bound, double upper_bound,int shape, BoundaryType lower_bc, BoundaryType upper_bc)
    : Mesh(lower_bound, upper_bound, shape, lower_bc, upper_bc)
    {
    validateBoundaryCondition();
    }


std::shared_ptr<Mesh> SphericalMesh::clone() const
    {
    return std::make_shared<SphericalMesh>(*this);
    }

double SphericalMesh::area(int i) const
    {
    const double r = lower_bound(i);
    return 4.*M_PI*r*r;
    }

double SphericalMesh::volume() const
    {
    const double rlo = lower_bound();
    const double rhi = upper_bound();
    return (4.*M_PI/3.)*(rhi*rhi*rhi - rlo*rlo*rlo);
    }
    
double SphericalMesh::volume(int i) const
    {
    const double r_out = upper_bound(i);
    const double r_in = lower_bound(i);
    return (4.*M_PI/3.)*(r_out*r_out*r_out - r_in*r_in*r_in);
    }

double SphericalMesh::gradient(int /*i*/, double f_lo, double f_hi) const
    {
    return (f_hi-f_lo)/(step_);
    }   
    
void SphericalMesh::validateBoundaryCondition() const
    {
    Mesh::validateBoundaryCondition();
    
    if(lower_bc_ == BoundaryType::periodic || upper_bc_ == BoundaryType::periodic)
        {
        throw std::invalid_argument("Periodic boundary conditions invalid in spherical geometry");
        }
    else if  (upper_bc_ == BoundaryType::reflect)
        {
        throw std::invalid_argument("Reflect boundary condition invalid for upper boundary");
        }
    }
}
