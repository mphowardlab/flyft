#include "_flyft.h"
#include "flyft/linear_potential.h"

void bindLinearPotential(py::module_& m)
    {
    using namespace flyft;

    py::class_<LinearPotential,std::shared_ptr<LinearPotential>,ExternalPotential>(m, "LinearPotential")
        .def(py::init<>())
        .def_property_readonly("xs", py::overload_cast<>(&LinearPotential::getXs), py::return_value_policy::reference_internal)
        .def_property_readonly("ys", py::overload_cast<>(&LinearPotential::getYs), py::return_value_policy::reference_internal)
        .def_property_readonly("slopes", py::overload_cast<>(&LinearPotential::getSlopes), py::return_value_policy::reference_internal)
        ;
    }
