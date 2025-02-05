#include "flyft/rosenfeld_fmt.h"

#include "_flyft.h"
#include "flyft/functional.h"

void bindRosenfeldFMT(py::module_& m)
    {
    using namespace flyft;

    py::class_<RosenfeldFMT, std::shared_ptr<RosenfeldFMT>, Functional>(m, "RosenfeldFMT")
        .def(py::init())
        .def_property_readonly("diameters",
                               py::overload_cast<>(&RosenfeldFMT::getDiameters),
                               py::return_value_policy::reference_internal);
    }
