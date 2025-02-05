#ifndef FLYFT_CARTESIAN_MESH_H_
#define FLYFT_CARTESIAN_MESH_H_

#include "flyft/mesh.h"

namespace flyft
    {

class CartesianMesh : public Mesh
    {
    public:
    CartesianMesh(double lower_bound,
                  double upper_bound,
                  int shape,
                  BoundaryType lower_bc,
                  BoundaryType upper_bc,
                  double area);

    double area(int i) const override;
    double volume() const override;
    double volume(int i) const override;
    double gradient(int idx, double f_lo, double f_hi) const override;

    protected:
    std::shared_ptr<Mesh> clone() const override;

    private:
    double area_; //<! Cross sectional area
    };
    } // namespace flyft
#endif // FLYFT_CARTESIAN_MESH_H_
