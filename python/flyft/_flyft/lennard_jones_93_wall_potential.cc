#include "flyft/lennard_jones_93_wall_potential.h"

#include "_flyft.h"
#include "wall_potential.h"

void bindLennardJones93WallPotential(py::module_& m)
    {
    using namespace flyft;

    bindWallPotential<LennardJones93WallPotential>(m, "LennardJones93WallPotential")
        .def_property_readonly("epsilons",
                               py::overload_cast<>(&LennardJones93WallPotential::getEpsilons),
                               py::return_value_policy::reference_internal)
        .def_property_readonly("sigmas",
                               py::overload_cast<>(&LennardJones93WallPotential::getSigmas),
                               py::return_value_policy::reference_internal)
        .def_property_readonly("cutoffs",
                               py::overload_cast<>(&LennardJones93WallPotential::getCutoffs),
                               py::return_value_policy::reference_internal)
        .def_property_readonly("shifts",
                               py::overload_cast<>(&LennardJones93WallPotential::getShifts),
                               py::return_value_policy::reference_internal);
    }
