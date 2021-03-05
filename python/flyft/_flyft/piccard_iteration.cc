#include "_flyft.h"
#include "flyft/piccard_iteration.h"

void bindPiccardIteration(py::module_& m)
    {
    using namespace flyft;

    py::class_<PiccardIteration,std::shared_ptr<PiccardIteration>,Solver>(m, "PiccardIteration")
        .def(py::init<std::shared_ptr<GrandPotential>,double,int,double>())
        .def_property("mix_parameter", &PiccardIteration::getMixParameter, &PiccardIteration::setMixParameter)
        .def_property("max_iterations", &PiccardIteration::getMaxIterations, &PiccardIteration::setMaxIterations)
        .def_property("tolerance", &PiccardIteration::getTolerance, &PiccardIteration::setTolerance)
        ;
    }
