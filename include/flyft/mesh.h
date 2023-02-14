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

        //! Get position on the mesh, defined as center of bin
        double coordinate(int i) const;

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
        
        double lower_bound(int i) const;
        double upper_bound(int i) const;
               

        double integrateSurface(int idx,const DataView<const double>& j) const;
        double integrateSurface(int idx,  const DataView<double>& j) const;
        double integrateSurface(int idx, double j_lo, double j_hi) const;
        
        double integrateVolume(int idx, const DataView<const double>& f) const;
        double integrateVolume(int idx, const DataView<double>& f) const;
        double integrateVolume(int idx, double f) const;
        
        double interpolate(int idx, const DataView<const double>& f) const;
        double interpolate(int idx, const DataView<double>& f) const;
        double interpolate(int idx, double f_lo, double f_hi) const; 
        
 
        virtual double area(int i) const = 0;//Crossectional area of the bin
        virtual double volume(int i) const = 0;
        virtual std::shared_ptr<Mesh> slice(int start, int end) const = 0;
        
        double gradient(int idx, const DataView<const double>& f) const ;
        double gradient(int idx, const DataView<double>& f) const ;
        virtual double gradient(int idx, double f_lo, double f_hi) const = 0;
        
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
