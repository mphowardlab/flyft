#include "_flyft.h"
#include "flyft/parallel_mesh.h"

void bindParallelMesh(py::module_& m)
    {
    using namespace flyft;

    py::class_<ParallelMesh,std::shared_ptr<ParallelMesh>>(m, "ParallelMesh")
        .def(py::init<std::shared_ptr<Mesh>,std::shared_ptr<Communicator>>())
        .def_property_readonly("local", &ParallelMesh::local)
        .def_property_readonly("full", &ParallelMesh::full)
        ;
    }
