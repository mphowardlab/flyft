#ifndef FLYFT_CARTESIAN_MESH_H_
#define FLYFT_CARTESIAN_MESH_H_

#include "flyft/mesh.h"

namespace flyft
{

class CartesianMesh: public Mesh
    {
    public: 
        CartesianMesh(double L, int shape);
        CartesianMesh(double L, int shape, double origin);
        CartesianMesh(int shape, double step);
        CartesianMesh(int shape, double step, double origin);
        
        std::shared_ptr<Mesh> slice(int start, int end) const override;
        
        double area(int i) const override;
        double volume(int i) const override;  
        double gradient(int idx, double f_lo, double f_hi) const override; 
    
    private:
        double area_; //<! Cross sectional area
    };
}
#endif // FLYFT_CARTESIAN_MESH_H_
