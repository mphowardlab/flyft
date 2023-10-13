#ifndef FLYFT_SPHERICAL_MESH_H_

#include "flyft/mesh.h"
#include <exception>

namespace flyft
{

class SphericalMesh : public Mesh
    {
    public:
        SphericalMesh(double lower_bound, double upper_bound, int shape, BoundaryType lower_bc, BoundaryType upper_bc);  

        double area(int i) const override;
        double volume() const override;
        double volume(int i) const override;
        double gradient(int idx, double f_lo, double f_hi) const override; 
    
    protected:
        void validateBoundaryCondition() const;
        std::shared_ptr<Mesh> clone() const override;
    };
}

#endif // FLYFT_SPHERICAL_MESH_H_
