#ifndef FLYFT_MESH_H_
#define FLYFT_MESH_H_

namespace flyft
{

class Mesh
    {
    public:
        Mesh() = delete;
        Mesh(double L, int shape);
        Mesh(double L, int shape, double buffer_request);

        //! Get position on the mesh, defined as center of bin
        double coordinate(int i) const;

        //! Get the bin for a coordinate
        int bin(double x) const;

        // Length of the mesh
        double L() const;

        //! Shape of the mesh
        int shape() const;

        //! Shape of the mesh buffer
        int buffer_shape() const;

        //! Total shape of the mesh, including buffer
        int buffered_shape() const;

        //! Step size of the mesh
        double step() const;

    private:
        double L_;      //!< Length of the domain
        int shape_;     //!< Number of mesh points
        double step_;   //!< Spacing between mesh points
        double buffer_;     //!< Buffer width
        int buffer_shape_;  //!< Number of mesh points in buffer
    };

}

#endif // FLYFT_MESH_H_
