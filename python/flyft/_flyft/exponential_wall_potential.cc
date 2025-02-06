#include "flyft/exponential_wall_potential.h"

#include "_flyft.h"
#include "wall_potential.h"

void bindExponentialWallPotential(py::module_& m)
    {
    using namespace flyft;

    bindWallPotential<ExponentialWallPotential>(m, "ExponentialWallPotential")
        .def_property_readonly("epsilons",
                               py::overload_cast<>(&ExponentialWallPotential::getEpsilons),
                               py::return_value_policy::reference_internal)
        .def_property_readonly("kappas",
                               py::overload_cast<>(&ExponentialWallPotential::getKappas),
                               py::return_value_policy::reference_internal)
        .def_property_readonly("shifts",
                               py::overload_cast<>(&ExponentialWallPotential::getShifts),
                               py::return_value_policy::reference_internal);
    }
