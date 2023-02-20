#include "_flyft.h"
#include "flyft/cartesian_mesh.h"

void bindSphericalMesh(py::module_& m)
    {
    using namespace flyft;

    py::class_<CartesianMesh,std::shared_ptr<CartesianMesh>,Mesh>(m, "CartesianMesh")
        .def(py::init<double,int,double>())
        .def(py::init<double,int,double,double>())
        .def(py::init<int,double,double>())
        .def(py::init<int,double,double,double>())
        ;
    }