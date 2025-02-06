#include "flyft/composite_external_potential.h"

#include "_flyft.h"
#include "flyft/external_potential.h"

#include <pybind11/stl.h>

void bindCompositeExternalPotential(py::module_& m)
    {
    using namespace flyft;

    py::class_<CompositeExternalPotential,
               std::shared_ptr<CompositeExternalPotential>,
               ExternalPotential>(m, "CompositeExternalPotential")
        .def(py::init())
        .def_property_readonly("objects",
                               &CompositeExternalPotential::getObjects,
                               py::return_value_policy::reference_internal)
        .def("append", &CompositeExternalPotential::addObject)
        .def("remove", &CompositeExternalPotential::removeObject)
        .def("clear", &CompositeExternalPotential::clearObjects);
    }
