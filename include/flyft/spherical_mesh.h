#ifndef FLYFT_SPHERICAL_MESH_H_

#include "flyft/mesh.h"

namespace flyft
{

class SphericalMesh : public Mesh
    {
    public:
        std::shared_ptr<Mesh> slice(int start, int end) const override;
        double volume(int i) const override;
    };
}

#endif // FLYFT_SPHERICAL_MESH_H_
