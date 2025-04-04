#ifndef FLYFT_MESH_H_
#define FLYFT_MESH_H_
#include "flyft/boundary_type.h"
#include "flyft/data_view.h"

#include <exception>
#include <memory>

namespace flyft
    {

class Mesh
    {
    public:
    Mesh() = delete;
    Mesh(double lower_bound,
         double upper_bound,
         int shape,
         BoundaryType lower_bc,
         BoundaryType upper_bc);

    std::shared_ptr<Mesh> slice(std::vector<int> start, std::vector<int> end) const;

    //! Get position on the mesh, defined as center of bin
    std::vector<double> center(std::vector<int> i) const;

    //! Lower bound of entire mesh
    std::vector<double> lower_bound() const;

    //! Get lower bound of bin
    std::vector<double> lower_bound(std::vector<int> i) const;

    //! Upper bound of entire mesh
    std::vector<double> upper_bound() const;

    //! Get upper bound of bin
    std::vector<double> upper_bound(int i) const;

    //! Get surface area of lower edge of bin
    virtual double area(int i) const = 0;

    //! Get volume of mesh
    virtual double volume() const = 0;

    //! Get volume of bin
    virtual double volume(int i) const = 0;

    //! Get the bin for a coordinate
    std::vector<int> bin(double x) const;

    //! Length of the mesh
    double L() const;

    //! Shape of the mesh
    std::vector<int> shape() const;

    //! Step size of the mesh
    std::vector<double> step() const;

    //! Boundary condition on lower bound of mesh
    BoundaryType lower_boundary_condition() const;

    //! Boundary condition on upper bound of mesh
    BoundaryType upper_boundary_condition() const;

    int asShape(double dx) const;

    double asLength(int shape) const;

    double integrateSurface(std::vector<int> idx, double j_lo, double j_hi) const;
    double integrateSurface(std::vector<int> idx, const DataView<double>& j) const;
    double integrateSurface(std::vector<int> idx, const DataView<const double>& j) const;

    double integrateVolume(std::vector<int> idx, double f) const;
    double integrateVolume(std::vector<int> idx, const DataView<double>& f) const;
    double integrateVolume(std::vector<int> idx, const DataView<const double>& f) const;

    template<typename T>
    typename std::remove_const<T>::type interpolate(double x, const DataView<T>& f) const;

    virtual double gradient(std::vector<int> idx, double f_lo, double f_hi) const = 0;
    double gradient(std::vector<int> idx, const DataView<const double>& f) const;
    double gradient(std::vector<int> idx, const DataView<double>& f) const;

    bool operator==(const Mesh& other) const;
    bool operator!=(const Mesh& other) const;

    protected:
    double lower_;
    std::vector<int> shape_; //!< Shape of the mesh
    BoundaryType lower_bc_;
    BoundaryType upper_bc_;
    double step_; //!< Spacing between mesh points
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
