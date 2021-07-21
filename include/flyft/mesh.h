#ifndef FLYFT_MESH_H_
#define FLYFT_MESH_H_

namespace flyft
{

class Mesh
    {
    public:
        Mesh() = delete;
        Mesh(double L, int shape);

        //! Get first index of mesh
        int begin() const;

        //! Get last index (exclusive) of mesh
        int end() const;

        //! Get first index of local mesh
        int first() const;

        //! Get last index (exclusive) of local mesh
        int last() const;

        //! Get position on the mesh, defined as center of bin
        double coordinate(int i) const;

        //! Get the bin for a coordinate
        int bin(double x) const;

        // Length of the mesh
        double L() const;

        //! Shape of the mesh
        int shape() const;

        //! Step size of the mesh
        double step() const;

    private:
        double L_;      //!< Length of the domain
        int shape_;     //!< Number of mesh points
        double step_;   //!< Spacing between mesh points
    };

}

#endif // FLYFT_MESH_H_
