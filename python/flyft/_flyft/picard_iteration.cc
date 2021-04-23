#include "_flyft.h"
#include "flyft/picard_iteration.h"

void bindPicardIteration(py::module_& m)
    {
    using namespace flyft;

    py::class_<PicardIteration,std::shared_ptr<PicardIteration>,Solver>(m, "PicardIteration")
        .def(py::init<double,int,double>())
        .def_property("mix_parameter", &PicardIteration::getMixParameter, &PicardIteration::setMixParameter)
        .def_property("max_iterations", &PicardIteration::getMaxIterations, &PicardIteration::setMaxIterations)
        .def_property("tolerance", &PicardIteration::getTolerance, &PicardIteration::setTolerance)
        ;
    }
