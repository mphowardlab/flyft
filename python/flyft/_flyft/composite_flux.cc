#include "_flyft.h"
#include "flyft/flux.h"
#include "flyft/composite_flux.h"

#include <pybind11/stl.h>

void bindCompositeFlux(py::module_& m)
    {
    using namespace flyft;

    py::class_<CompositeFlux,std::shared_ptr<CompositeFlux>,Flux>(m, "CompositeFlux")
        .def(py::init())
        .def_property_readonly("objects",&CompositeFlux::getObjects,py::return_value_policy::reference_internal)
        .def("append", &CompositeFlux::addObject)
        .def("remove", &CompositeFlux::removeObject)
        .def("clear", &CompositeFlux::clearObjects)
        ;
    }
