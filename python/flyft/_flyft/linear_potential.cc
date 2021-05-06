#include "_flyft.h"
#include "flyft/linear_potential.h"

void bindLinearPotential(py::module_& m)
    {
    using namespace flyft;

    py::class_<LinearPotential,std::shared_ptr<LinearPotential>,ExternalPotential>(m, "LinearPotential")
        .def(py::init<>())
        .def_property("xs", &LinearPotential::getXs, &LinearPotential::setXs)
        .def_property("ys", &LinearPotential::getYs, &LinearPotential::setYs)
        .def_property("slopes", &LinearPotential::getSlopes, &LinearPotential::setSlopes)
        ;
    }
