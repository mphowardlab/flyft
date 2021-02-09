#include "_flyft.h"
#include "flyft/mesh.h"

void bindMesh(py::module_& m)
    {
    using namespace flyft;

    py::class_<Mesh,std::shared_ptr<Mesh>>(m, "Mesh")
        .def(py::init<double,int>())
        .def_property_readonly("L", &Mesh::L)
        .def_property_readonly("shape", &Mesh::shape)
        .def_property_readonly("step", &Mesh::step)
        .def("coordinate", &Mesh::coordinate)
        .def("bin", &Mesh::bin);
    }
