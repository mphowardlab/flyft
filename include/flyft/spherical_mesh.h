#ifndef FLYFT_SPHERICAL_MESH_H_

#include "flyft/mesh.h"

namespace flyft
{

class SphericalMesh : public Mesh
    {
    public:
        // SphericalMesh(double lower, double upper, int shape, BoundaryType upper_bc);
        SphericalMesh(double lower, double upper, int shape, BoundaryType lower_bc, BoundaryType upper_bc);  

        
        std::shared_ptr<Mesh> slice(int start, int end) const override;

        double area(int i) const override;
        double volume() const override;
        double volume(int i) const override;
        double gradient(int idx, double f_lo, double f_hi) const override; 
    
    private:
        void validateBoundaryCondition();
    };
}

#endif // FLYFT_SPHERICAL_MESH_H_
