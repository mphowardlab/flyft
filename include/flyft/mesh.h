#ifndef FLYFT_MESH_H_
#define FLYFT_MESH_H_
#include "flyft/boundary_type.h"
#include "flyft/data_view.h"

#include <exception>
#include <memory>

namespace flyft
    {
//! Mesh
/*!
 * A mesh is a discretized coordinate space. It has one position coordinate and may optionally have
 * one, two, or three orientation coordinates. The position coordinate is a symmetry of a standard
 * three-dimensional coordinate system (e.g., Cartesian or spherical).
 *
 * The orientation coordinates are body-fixed Euler angles defined using the Z-X-Z convention for
 * the body's principal axes. The bounds of these angles are [0, 2pi), [0, pi], and [0, 2pi],
 * respectively.
 */
class Mesh
    {
    public:
    // No default constructor
    Mesh() = delete;

    //! Constructor
    /*!
     * \param num_orientation_dim Number of orientation coordinates.
     * \param shape Number of cells along each dimension.
     * \param lower_bound Lower bound of the mesh.
     * \param upper_bound Upper bound of the mesh.
     * \param lower_bc Boundary condition at position coordinate lower bound.
     * \param upper_bc Boundary condition at position coordinate upper bound.
     */
    Mesh(int num_orientation_dim,
         int* shape,
         double lower_bound,
         double upper_bound,
         BoundaryType lower_bc,
         BoundaryType upper_bc);

    //! Copy constructor.
    Mesh(const Mesh& other);

    //! Copy assignment operator.
    Mesh& operator=(const Mesh& other);

    //! Move constructor.
    Mesh(const Mesh&& other);

    //! Move assignment operator.
    Mesh& operator=(const Mesh&& other);

    //! Destructor
    virtual ~Mesh();

    //! Slice mesh by position coordinate.
    /*!
     * \param start First position index to include.
     * \param end First position index to exclude.
     * \return Slice of original mesh.
     *
     * A mesh may not be sliced more than once.
     */
    std::shared_ptr<Mesh> slice(int start, int end) const;

    //! Calculate lower bound of cell.
    /*!
     * \param coordinate Lower bound of cell.
     * \param cell Cell multi-index.
     */
    void cell_lower_bound(double* coordinate, const int* cell) const;

    //! Calculate lower bound of cell in a given dimension.
    /*!
     * \param dimension Dimension of coordinate.
     * \param index Cell index in given dimension.
     * \returns Lower bound of cell in specified dimension.
     */
    double cell_lower_bound(int dimension, int index) const;

    //! Calculate lower bound of cell in position space.
    /*!
     * \param coordinate Lower bound of cell in position space.
     * \param cell Cell multi-index.
     */
    void cell_position_lower_bound(double* coordinate, const int* cell) const;

    //! Calculate lower bound of cell in orientation space.
    /*!
     * \param coordinate Lower bound of cell in orientation space.
     * \param cell Cell multi-index.
     */
    void cell_orientation_lower_bound(double* coordinate, const int* cell) const;

    //! Calculate center of cell.
    /*!
     * \param coordinate Center of cell.
     * \param cell Cell multi-index.
     */
    void cell_center(double* coordinate, const int* cell) const;

    //! Calculate center of cell in a given dimension.
    /*!
     * \param dimension Dimension of coordinate.
     * \param index Cell index in given dimension.
     * \returns Center of cell in specified dimension.
     */
    double cell_center(int dimension, int index) const;

    //! Calculate center of cell in position space.
    /*!
     * \param coordinate Center of cell in position space.
     * \param cell Cell multi-index.
     */
    void cell_position_center(double* coordinate, const int* cell) const;

    //! Calculate center of cell in orientation space.
    /*!
     * \param coordinate Center of cell in orientation space.
     * \param cell Cell multi-index.
     */
    void cell_orientation_center(double* coordinate, const int* cell) const;

    //! Calculate upper bound of cell.
    /*!
     * \param coordinate Upper bound of cell.
     * \param cell Cell multi-index.
     */
    void cell_upper_bound(double* coordinate, const int* cell) const;

    //! Calculate upper bound of cell in a given dimension.
    /*!
     * \param dimension Dimension of coordinate.
     * \param index Cell index in given dimension.
     * \returns upper bound of cell in specified dimension.
     */
    double cell_upper_bound(int dimension, int index) const;

    //! Calculate upper bound of cell in position space.
    /*!
     * \param coordinate Upper bound of cell in position space.
     * \param cell Cell multi-index.
     */
    void cell_position_upper_bound(double* coordinate, const int* cell) const;

    //! Calculate upper bound of cell in orientation space.
    /*!
     * \param coordinate Upper bound of cell in orientation space.
     * \param cell Cell multi-index.
     */
    void cell_orientation_upper_bound(double* coordinate, const int* cell) const;

#if 0
    //! Get surface area of lower edge of bin
    /*!
     * \param i Multidimensional bin index
     * \return Upper bound of the multidimensional bin
     */
    virtual double area(int i) const = 0;
#endif

    //! Calculate volume of cell.
    /*!
     * \param cell Cell multi-index.
     * \return Volume of cell.
     *
     * When orientation coordinates are present, the cell volume is a hypervolume over position and
     * orientation space.
     */
    double cell_volume(const int* cell) const;

    //! Calculate volume of cell in position space.
    /*!
     * \param cell Cell multi-index.
     * \return Volume of cell in position space.
     *
     * The position coordinates reflect symmetries of the three-dimensional coordinate system, so
     * this quantity is always a volume regardless of the number of coordinates.
     */
    virtual double cell_position_volume(const int* cell) const = 0;

    //! Calculate volume of cell in orientation space.
    /*!
     * \param cell Cell multi-index.
     * \return Volume of cell in orientation space.
     *
     * Orientation coordinates are only present as required, so this volume is actually a solid
     * angle over valid coordinates. It is zero if there are no orientation coordinates.
     */
    double cell_orientation_volume(const int* cell) const;

    //! Calculate the cell that contains a coordinate.
    /*!
     * \param cell Cell multi-index.
     * \param coordinate Coordinate.
     */
    void compute_cell(int* cell, const double* coordinate) const;

    //! Convert a length to a number of cells.
    /*!
     * \param shape Number of cells per dimension.
     * \param length Length per dimension.
     */
    void length_as_shape(int* shape, const double* length) const;

    //! Convert a shape to a length.
    /*!
     * \param length Length per dimension.
     * \param shape Number of cells per dimension.
     */
    void shape_as_length(double* length, const int* shape) const;

    //! Number of position coordinates.
    int num_position_coordinates() const;

    //! Number of orientation coordinates.
    int num_orientation_coordinates() const;

    //! Total number of coordinates.
    int num_coordinates() const;

    //! Number of cells per dimension.
    const int* shape() const;

    //! Size of cell per dimension.
    const double* step() const;

    //! Boundary condition at position coordinate lower bound.
    BoundaryType lower_boundary_condition() const;

    //! Boundary condition at position coordinate upper bound.
    BoundaryType upper_boundary_condition() const;

#if 0
    //! Get the integral of the surface over the mesh
    /*!
     * \param shape Shape of the mesh
     * \return Length of the mesh
     */
    double integrateSurface(const std::vector<int>& idx, double j_lo, double j_hi) const;
    double integrateSurface(const std::vector<int>& idx, const DataView<double>& j) const;
    double integrateSurface(const std::vector<int>& idx, const DataView<const double>& j) const;
#endif

    //! Integrate function over cell volume.
    /*!
     * \param cell Cell multi-index.
     * \param f Discretized function to integrate.
     * \return Integral of the function over the cell volume.
     *
     * The integration is performed using a simple midpoint method.
     */
    template<class T>
    double integrate_cell_volume(const int* cell, const T& f) const
        {
        return cell_volume(cell) * f(cell);
        }

    //! Integrate function over cell orientation volume.
    /*!
     * \param cell Cell multi-index.
     * \param f Discretized function to integrate.
     * \return Integral of the function over the cell orientation volume.
     *
     * The integration is performed using a simple midpoint method.
     */
    template<class T>
    double integrate_cell_orientation_volume(const int* cell, const T& f) const
        {
        return cell_orientation_volume(cell) * f(cell);
        }

#if 0
    //! Interpolate a function on the mesh
    /*!
     * \param x Coordinate position of the bin
     * \param f Function to interpolate
     * \return Interpolated function value
     *
     * The function is interpolated using linear interpolation.
     */
    template<typename T>
    typename std::remove_const<T>::type interpolate(double x, const DataView<T>& f) const;

    //! Get the gradient of the mesh
    virtual double gradient(const std::vector<int>& idx, double f_lo, double f_hi) const = 0;
    double gradient(const std::vector<int>& idx, const DataView<const double>& f) const;
    double gradient(const std::vector<int>& idx, const DataView<double>& f) const;
#endif

    //! Check if two meshes are equivalent.
    /*!
     * The meshes are identical if they have the same type, same numbers of coordinates, same
     * coordinate space, same shape, and same boundary conditions.
     */
    bool operator==(const Mesh& other) const;

    //! Check if two meshes are not equivalent.
    bool operator!=(const Mesh& other) const;

    //! Check if boundary conditions are valid.
    virtual bool validate_boundary_conditions() const;

    protected:
    const int num_position_dim_ = 1; //!< Number of position coordinates
    int num_orientation_dim_;        //!< Number of orientation coordinates
    int num_dim_;                    //!< Total number of coordinates
    int* shape_;                     //!< Shape of the mesh
    int start_;                      //!< First index in mesh

    double* lower_; //!< Lower bounds of coordinates
    double* step_;  //!< Spacing between mesh points

    BoundaryType lower_bc_; //!< Boundary condition of the lower bound
    BoundaryType upper_bc_; //!< Boundary condition of the upper bound

    virtual std::shared_ptr<Mesh> clone() const = 0;

    private:
    //! Allocate per dimension memory.
    void allocate();
    };

#if 0
template<typename T>
typename std::remove_const<T>::type Mesh::interpolate(double x, const DataView<T>& f) const
    {
    const auto idx = bin(x);
    const auto x_c = center(idx);
    double x_0, x_1;
    typename std::remove_const<T>::type f_0, f_1;
    if (x < x_c)
        {
        x_0 = center(idx - 1);
        x_1 = x_c;
        f_0 = f(idx - 1);
        f_1 = f(idx);
        }
    else
        {
        x_0 = x_c;
        x_1 = center(idx + 1);
        f_0 = f(idx);
        f_1 = f(idx + 1);
        }

    return f_0 + (x - x_0) * (f_1 - f_0) / (x_1 - x_0);
    }
#endif

    } // namespace flyft

#endif // FLYFT_MESH_H_
