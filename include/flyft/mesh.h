#ifndef FLYFT_MESH_H_
#define FLYFT_MESH_H_
#include "flyft/data_view.h"

#include <memory>

namespace flyft
{

class Mesh
    {
    public:
        Mesh() = delete;
        Mesh(double L, int shape);
        Mesh(double L, int shape, double origin);
        Mesh(int shape, double step);
        Mesh(int shape, double step, double origin);

        virtual std::shared_ptr<Mesh> slice(int start, int end) const = 0;

        //! Get position on the mesh, defined as center of bin
        double center(int i) const;

        //! Get lower bound of bin
        double lower_bound(int i) const;

        //! Get upper bound of bin
        double upper_bound(int i) const;

        //! Get surface area of lower edge of bin
        virtual double area(int i) const = 0;

        //! Get volume of mesh
        virtual double volume() const = 0;

        //! Get volume of bin
        virtual double volume(int i) const = 0;

        //! Get the bin for a coordinate
        int bin(double x) const;

        //! Length of the mesh
        double L() const;

        //! Shape of the mesh
        int shape() const;

        //! Step size of the mesh
        double step() const;

        //! Mesh origin
        double origin() const;

        int asShape(double dx) const;

        double asLength(int shape) const;

        double integrateSurface(int idx, double j_lo, double j_hi) const;
        double integrateSurface(int idx, const DataView<double>& j) const;
        double integrateSurface(int idx, const DataView<const double>& j) const;

        double integrateVolume(int idx, double f) const;
        double integrateVolume(int idx, const DataView<double>& f) const;
        double integrateVolume(int idx, const DataView<const double>& f) const;

        double interpolate(int idx, double f_lo, double f_hi) const;
        double interpolate(int idx, const DataView<double>& f) const;  
        double interpolate(int idx, const DataView<const double>& f) const;      
        
        virtual double gradient(int idx, double f_lo, double f_hi) const = 0;
        double gradient(int idx, const DataView<const double>& f) const ;
        double gradient(int idx, const DataView<double>& f) const ;
        
        bool operator==(const Mesh& other) const;
        bool operator!=(const Mesh& other) const;

    protected:
        double L_;      //!< Length of the domain
        int shape_;     //!< Shape of the mesh
        double step_;   //!< Spacing between mesh points
        double origin_; //!< Origin of the mesh
    };

}

#endif // FLYFT_MESH_H_
