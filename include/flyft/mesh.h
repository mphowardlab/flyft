#ifndef FLYFT_MESH_H_
#define FLYFT_MESH_H_

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

        //! Shape of the mesh
        int shape() const;

        //! Step size of the mesh
        double step() const;

        int asShape(double dx) const;

        double asLength(int shape) const;

    private:
        double L_;      //!< Length of the domain
        int shape_;     //!< Shape of the mesh
        double step_;   //!< Spacing between mesh points
    };

}

#endif // FLYFT_MESH_H_
