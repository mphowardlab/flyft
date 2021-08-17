#include "_flyft.h"
#include "flyft/harmonic_wall_potential.h"

void bindHarmonicWallPotential(py::module_& m)
    {
    using namespace flyft;

    py::class_<HarmonicWallPotential,std::shared_ptr<HarmonicWallPotential>,WallPotential>(m, "HarmonicWallPotential")
        .def(py::init<double,double>())
        .def(py::init<std::shared_ptr<DoubleParameter>,double>())
        .def_property_readonly("spring_constants", py::overload_cast<>(&HarmonicWallPotential::getSpringConstants), py::return_value_policy::reference_internal)
        .def_property_readonly("shifts", py::overload_cast<>(&HarmonicWallPotential::getShifts), py::return_value_policy::reference_internal)
        ;
    }
