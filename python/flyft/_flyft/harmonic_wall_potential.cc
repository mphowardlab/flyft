#include "_flyft.h"
#include "wall_potential.h"
#include "flyft/harmonic_wall_potential.h"

void bindHarmonicWallPotential(py::module_& m)
    {
    using namespace flyft;

    bindWallPotential<HarmonicWallPotential>(m,"HarmonicWallPotential")
        .def_property_readonly("spring_constants", py::overload_cast<>(&HarmonicWallPotential::getSpringConstants), py::return_value_policy::reference_internal)
        .def_property_readonly("shifts", py::overload_cast<>(&HarmonicWallPotential::getShifts), py::return_value_policy::reference_internal)
        ;
    }
