#ifndef FLYFT_CARTESIAN_MESH_H_
#define FLYFT_CARTESIAN_MESH_H_

#include "flyft/mesh.h"

namespace flyft
{

class CartesianMesh: public Mesh
    {
    public: 
        double volume(int i) const override;
    };

}
#endif // FLYFT_CARTESIAN_MESH_H_


