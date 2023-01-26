#ifndef FLYFT_SPHERICAL_MESH_H_
#ifndef FLYFT_SPHERICAL_MESH_H_

#include "flyft/mesh.h"

namespace flyft
{

class SphericalMesh : public Mesh
    {
    public:
        double volume(int i) const override;
    };
}

#endif // FLYFT_SPHERICAL_MESH_H_
