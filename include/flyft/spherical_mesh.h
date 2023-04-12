#ifndef FLYFT_SPHERICAL_MESH_H_

#include "flyft/mesh.h"

namespace flyft
{

class SphericalMesh : public Mesh
    {
    public:
        SphericalMesh(double R, int shape);
        SphericalMesh(double R, int shape, double origin);  
        SphericalMesh(int shape, double step);  
        SphericalMesh(int shape, double step,double origin);  
        
        std::shared_ptr<Mesh> slice(int start, int end) const override;
        
        double area(int i) const override;
        double volume() const override;
        double volume(int i) const override;
        double gradient(int idx, double f_lo, double f_hi) const override; 
    };
}

#endif // FLYFT_SPHERICAL_MESH_H_
