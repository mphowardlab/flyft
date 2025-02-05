#include "flyft/composite_functional.h"

#include "_flyft.h"
#include "flyft/functional.h"

#include <pybind11/stl.h>

void bindCompositeFunctional(py::module_& m)
    {
    using namespace flyft;

    py::class_<CompositeFunctional, std::shared_ptr<CompositeFunctional>, Functional>(
        m,
        "CompositeFunctional")
        .def(py::init())
        .def_property_readonly("objects",
                               &CompositeFunctional::getObjects,
                               py::return_value_policy::reference_internal)
        .def("append", &CompositeFunctional::addObject)
        .def("remove", &CompositeFunctional::removeObject)
        .def("clear", &CompositeFunctional::clearObjects);
    }
