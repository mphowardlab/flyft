#include "flyft/spherical_mesh.h"

#include "_flyft.h"

void bindSphericalMesh(py::module_& m)
    {
    using namespace flyft;

    py::class_<SphericalMesh, std::shared_ptr<SphericalMesh>, Mesh>(m, "SphericalMesh")
        .def(py::init<double, double, int, BoundaryType, BoundaryType>());
    }
