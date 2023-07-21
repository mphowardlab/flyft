#include "flyft/cartesian_mesh.h"
#include <iostream>

namespace flyft
{

CartesianMesh::CartesianMesh(double lower,double upper,int shape, BoundaryType lower_bc, BoundaryType upper_bc,double area)
    : Mesh(lower,upper,shape,lower_bc,upper_bc), area_(area)
    {
    }

std::shared_ptr<Mesh> CartesianMesh::slice(int start, int end) const
    {
    return std::shared_ptr<Mesh>(new CartesianMesh(lower_bound(start),lower_bound(end),end-start, (start > 0)?BoundaryType::internal:lower_bc_, (start > 0)?BoundaryType::internal:upper_bc_,area_));
    }

double CartesianMesh::area(int /*i*/) const
    {
    return area_;
    }

double CartesianMesh::volume() const
    {
    return area_*L_;
    }

double CartesianMesh::volume(int /*i*/) const
    {
    return area_*step_;
    }

double CartesianMesh::gradient(int /*i*/, double f_lo, double f_hi) const
    {
    return (f_hi-f_lo)/step_; 
    }

void CartesianMesh::validateBoundaryCondition(){
    if ((lower_bc_ != BoundaryType::periodic && upper_bc_ == BoundaryType::periodic) ||
        (lower_bc_ == BoundaryType::periodic && upper_bc_ != BoundaryType::periodic))
        {
        throw std::invalid_argument("Both boundaries must be periodic if one is.");
        }
    }        
}


