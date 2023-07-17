#include "_flyft.h"
#include "flyft/spherical_mesh.h"

void bindSphericalMesh(py::module_& m)
    {
    using namespace flyft;

    py::class_<SphericalMesh,std::shared_ptr<SphericalMesh>,Mesh>(m, "SphericalMesh")
        .def(py::init<double,int,BoundaryType>())
        .def(py::init<double, int, double,BoundaryType,BoundaryType>())
        .def(py::init<int,double>())
        .def(py::init<int,double,double>())
        ;
    }
