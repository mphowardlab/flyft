#include "_flyft.h"
#include "flyft/brownian_diffusive_flux.h"

#include <pybind11/stl.h>

void bindBrownianDiffusiveFlux(py::module_& m)
    {
    using namespace flyft;

    py::class_<BrownianDiffusiveFlux,std::shared_ptr<BrownianDiffusiveFlux>,DiffusiveFlux>(m, "BrownianDiffusiveFlux")
        .def(py::init<>())
        ;
    }
