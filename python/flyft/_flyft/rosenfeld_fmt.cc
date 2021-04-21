#include "_flyft.h"
#include "flyft/functional.h"
#include "flyft/rosenfeld_fmt.h"

void bindRosenfeldFMT(py::module_& m)
    {
    using namespace flyft;

    py::class_<RosenfeldFMT,std::shared_ptr<RosenfeldFMT>,Functional>(m, "RosenfeldFMT")
        .def(py::init())
        .def_property("diameters", &RosenfeldFMT::getDiameters, &RosenfeldFMT::setDiameters, py::return_value_policy::reference_internal)
        ;
    }
