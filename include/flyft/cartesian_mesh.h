#ifndef FLYFT_CARTESIAN_MESH_H_
#define FLYFT_CARTESIAN_MESH_H_

#include "flyft/mesh.h"

namespace flyft
    {

//! Mesh with Cartesian position coordinates.
class CartesianMesh : public Mesh
    {
    public:
    //! Constructor.
    /*!
     * \param num_orientation_dim Number of orientation coordinates.
     * \param shape Number of cells along each dimension.
     * \param lower_bound Lower bound of the mesh.
     * \param upper_bound Upper bound of the mesh.
     * \param lower_bc Boundary condition at position coordinate lower bound.
     * \param upper_bc Boundary condition at position coordinate upper bound.
     * \param area Cross-sectional area, tangent to position coordinate.
     */
    CartesianMesh(int num_orientation_dim,
                  int* shape,
                  double lower_bound,
                  double upper_bound,
                  BoundaryType lower_bc,
                  BoundaryType upper_bc,
                  double area);

    //! Copy constructor.
    CartesianMesh(const CartesianMesh& other);

    //! Copy assignment operator.
    CartesianMesh& operator=(const CartesianMesh& other);

    //! Move constructor.
    CartesianMesh(const CartesianMesh&& other);

    //! Move assignment operator.
    CartesianMesh& operator=(const CartesianMesh&& other);

    // double area(int i) const override;
    double cell_position_volume(const int* cell) const override;
    // double gradient(int idx, double f_lo, double f_hi) const override;

    protected:
    std::shared_ptr<Mesh> clone() const override;

    private:
    double area_; //<! Cross sectional area
    };
    } // namespace flyft
#endif // FLYFT_CARTESIAN_MESH_H_
