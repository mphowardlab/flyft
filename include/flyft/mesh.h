#ifndef FLYFT_MESH_H_
#define FLYFT_MESH_H_

#include "flyft/data_layout.h"

namespace flyft
{

class Mesh
    {
    public:
        Mesh() = delete;
        Mesh(double L, int shape);

        //! Get position on the mesh, defined as center of bin
        double coordinate(int i) const;

        //! Get the bin for a coordinate
        int bin(double x) const;

        //! Length of the mesh
        double L() const;

        //! Length of the mesh buffer
        double buffer() const;

        //! Step size of the mesh
        double step() const;

        const DataLayout& layout() const;

        //! Shape of the mesh
        int shape() const;

        //! Shape of the mesh buffer
        int buffer_shape() const;

        //! Shape of the mesh and buffer
        int full_shape() const;

        void setBuffer(double buffer_request);

    private:
        double L_;      //!< Length of the domain
        double buffer_; //!< Size of buffer
        double step_;   //!< Spacing between mesh points
        DataLayout layout_;
    };

}

#endif // FLYFT_MESH_H_
