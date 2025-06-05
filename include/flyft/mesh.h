#ifndef FLYFT_MESH_H_
#define FLYFT_MESH_H_
#include "flyft/boundary_type.h"
#include "flyft/data_view.h"

#include <exception>
#include <memory>

namespace flyft
    {
//! Multidimensional mesh
/*!
 * Multidimensional mesh maps N-dimensional indexing for
 * the anistropic particles into one-dimensional flux.
 */
class Mesh
    {
    public:
    //! No default constructor
    Mesh() = delete;

    //! Constructor
    /*!
     * \param lower_bound Lower bound of the N-dimensional mesh.
     * \param upper_bound Upper bound of the N-dimensional mesh.
     * \param shape Shape of the index array.
     * \param lower_bc Boundary condition of the lower bound.
     * \param upper_bc Boundary condition of the upper bound.
     */
    Mesh(double lower_bound,
         double upper_bound,
         int shape,
         BoundaryType lower_bc,
         BoundaryType upper_bc);

    //! Slice multidimensional index
    /*!
     * \param start Multidimensional start index.
     * \param end Multidimensional end index.
     * \return One-dimensional index.
     *
     */
    std::shared_ptr<Mesh> slice(const std::vector<int>& start, const std::vector<int>& end) const;

    //! Get position on the mesh, defined as center of bin
    /*!
     * \param i Multidimensional bin index
     * \return Center of the multidimensional bin
     */
    std::vector<double> center(const std::vector<int>& i) const;

    //! Lower bound of entire mesh
    /*!
     * \return Lower bound of the multidimensional mesh
     */
    std::vector<double> lower_bound() const;

    //! Get lower bound of bin
    /*!
     * \param i Multidimensional bin index
     * \return Lower bound of the multidimensional bin
     */
    std::vector<double> lower_bound(const std::vector<int>& i) const;

    //! Upper bound of entire mesh
    /*!
     * \return Upper bound of the multidimensional mesh
     */
    std::vector<double> upper_bound() const;

    //! Get upper bound of bin
    /*!
     * \param i Multidimensional bin index
     * \return Upper bound of the multidimensional bin
     */
    std::vector<double> upper_bound(const std::vector<int>& i) const;

    //! Get surface area of lower edge of bin
    /*!
     * \param i Multidimensional bin index
     * \return Upper bound of the multidimensional bin
     */
    virtual double area(int i) const = 0;

    //! Get volume of mesh
    /*!
     * \return Volume of the mesh
     */
    virtual double volume() const = 0;

    //! Get volume of bin
    /*!
     * \param i Multidimensional bin index
     * \return Volume of the bin
     */
    virtual std::vector<double> volume(int i) const = 0;

    //! Get the bin for a coordinate
    /*!
     * \param x Coordinate position of the bin
     * \return Bin index
     */
    std::vector<int> bin(const std::vector<double>& x) const;

    //! Length of the mesh
    /*!
     * \return Length of the mesh
     */
    std::vector<double> L() const;

    //! Shape of the mesh
    std::vector<int> shape() const;

    //! Step size of the mesh
    std::vector<double> step() const;

    //! Boundary condition on lower bound of mesh
    BoundaryType lower_boundary_condition() const;

    //! Boundary condition on upper bound of mesh
    BoundaryType upper_boundary_condition() const;

    //! Get the start index of the mesh
    /*!
     * \param dx Step size of the mesh
     * \return Start index of the mesh
     */
    std::vector<int> asShape(const std::vector<double>& dx) const;

    //! Get the length of the mesh
    /*!
     * \param shape Shape of the mesh
     * \return Length of the mesh
     */
    std::vector<double> asLength(const std::vector<int>& shape) const;

    //! Get the integral of the surface over the mesh
    /*!
     * \param shape Shape of the mesh
     * \return Length of the mesh
     */
    double integrateSurface(const std::vector<int>& idx, double j_lo, double j_hi) const;
    double integrateSurface(const std::vector<int>& idx, const DataView<double>& j) const;
    double integrateSurface(const std::vector<int>& idx, const DataView<const double>& j) const;

    //! Get the integral of the volume over the mesh
    /*!
     * \param idx Multidimensional index
     * \param f Function to integrate
     * \return Integral of the function over the mesh
     */
    double integrateVolume(const std::vector<int>& idx, double f) const;
    double integrateVolume(const std::vector<int>& idx, const DataView<double>& f) const;
    double integrateVolume(const std::vector<int>& idx, const DataView<const double>& f) const;

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

    bool operator==(const Mesh& other) const;
    bool operator!=(const Mesh& other) const;

    protected:
    std::vector<double> lower_;
    std::vector<int> shape_;   //!< Shape of the mesh
    BoundaryType lower_bc_;    //!< Boundary condition of the lower bound
    BoundaryType upper_bc_;    //!< Boundary condition of the upper bound
    std::vector<double> step_; //!< Spacing between mesh points
    std::vector<int> start_;

    void validateBoundaryCondition() const;

    virtual std::shared_ptr<Mesh> clone() const = 0;
    };

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

    } // namespace flyft

#endif // FLYFT_MESH_H_
