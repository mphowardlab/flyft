#ifndef FLYFT_CARTESIAN_MESH_H_
#define FLYFT_CARTESIAN_MESH_H_

#include "flyft/mesh.h"

namespace flyft
{

class CartesianMesh: public Mesh
    {
    public: 
        CartesianMesh(double L, int shape);
        double volume(int i) const override;
       
    
    private:
        double area_; //Crossectional area of the bin
    };
}
#endif // FLYFT_CARTESIAN_MESH_H_


