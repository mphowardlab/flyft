#include "_flyft.h"
#include "flyft/rpy_diffusive_flux.h"

#include <pybind11/stl.h>

void bindRPYDiffusiveFlux(py::module_& m)
    {
    using namespace flyft;

    py::class_<RPYDiffusiveFlux,std::shared_ptr<RPYDiffusiveFlux>,Flux>(m, "RPYDiffusiveFlux")
        .def(py::init<>())
        .def_property_readonly("diameters",py::overload_cast<>(&RPYDiffusiveFlux::getDiameters), py::return_value_policy::reference_internal)
        .def_property_readonly("viscosity",&RPYDiffusiveFlux::getViscosity)
        ;
    }
