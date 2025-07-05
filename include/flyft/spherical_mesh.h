#ifndef FLYFT_SPHERICAL_MESH_H_

#include "flyft/mesh.h"

#include <exception>

namespace flyft
    {

class SphericalMesh : public Mesh
    {
    public:
    // double area(int i) const override;
    double cell_position_volume(const int* cell) const override;
    // double gradient(int idx, double f_lo, double f_hi) const override;

    bool validate_boundary_conditions() const override;

    protected:
    std::shared_ptr<Mesh> clone() const override;
    };
    } // namespace flyft

#endif // FLYFT_SPHERICAL_MESH_H_
