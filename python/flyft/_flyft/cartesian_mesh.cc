#include "_flyft.h"
#include "flyft/cartesian_mesh.h"

void bindCartesianMesh(py::module_& m)
    {
    using namespace flyft;

    py::class_<CartesianMesh,std::shared_ptr<CartesianMesh>,Mesh>(m, "CartesianMesh")
        .def(py::init<double,double,int,BoundaryType,BoundaryType,double>())
        ;
    }
