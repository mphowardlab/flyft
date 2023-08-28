#include "_flyft.h"
#include "flyft/brownian_diffusive_flux.h"

#include <pybind11/stl.h>

void bindBrownianDiffusiveFlux(py::module_& m)
    {
    using namespace flyft;

    py::class_<BrownianDiffusiveFlux,std::shared_ptr<BrownianDiffusiveFlux>,Flux>(m, "BrownianDiffusiveFlux")
        .def(py::init<>())
        .def_property_readonly("diffusivities",py::overload_cast<>(&BrownianDiffusiveFlux::getDiffusivities), py::return_value_policy::reference_internal)
        ;
    }
