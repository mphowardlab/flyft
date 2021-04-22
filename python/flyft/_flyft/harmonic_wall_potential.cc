#include "_flyft.h"
#include "flyft/harmonic_wall_potential.h"

void bindHarmonicWallPotential(py::module_& m)
    {
    using namespace flyft;

    py::class_<HarmonicWallPotential,std::shared_ptr<HarmonicWallPotential>,WallPotential>(m, "HarmonicWallPotential")
        .def(py::init<double,double>())
        .def_property("spring_constants", &HarmonicWallPotential::getSpringConstants, &HarmonicWallPotential::setSpringConstants)
        .def_property("shifts", &HarmonicWallPotential::getShifts, &HarmonicWallPotential::setShifts)
        ;
    }
