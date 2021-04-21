#ifndef FLYFT_RECIPROCAL_MESH_H_
#define FLYFT_RECIPROCAL_MESH_H_

#include "flyft/mesh.h"

namespace flyft
{

class ReciprocalMesh
    {
    public:
        ReciprocalMesh() = delete;
        ReciprocalMesh(double L, int shape);
        ReciprocalMesh(const Mesh& mesh);

        //! Get position on the mesh
        double coordinate(int i) const;

        //! Length of the mesh
        double L() const;

        //! Shape the mesh
        int shape() const;

        //! Step size of the mesh
        double step() const;

    private:
        double L_;      //!< Length of the domain
        int shape_;     //!< Number of mesh points
        double step_;   //!< Spacing between mesh points
        int mid_bin_;   //!< Middle bin for frequencies to change sign
    };

}
#endif // FLYFT_RECIPROCAL_MESH_H_
