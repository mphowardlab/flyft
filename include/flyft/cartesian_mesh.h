#ifndef FLYFT_CARTESIAN_MESH_H_
#define FLYFT_CARTESIAN_MESH_H_

#include "flyft/mesh.h"

namespace flyft
{

class CartesianMesh: public Mesh
    {
    public: 
    
        double volume(int i) const override;
        virtual double defaultArea() const;
    
    private:
        double area_; //Crossectional area of the bin
    };
}
#endif // FLYFT_CARTESIAN_MESH_H_


