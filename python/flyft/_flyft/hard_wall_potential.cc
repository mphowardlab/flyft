#include "_flyft.h"
#include "wall_potential.h"
#include "flyft/hard_wall_potential.h"

void bindHardWallPotential(py::module_& m)
    {
    using namespace flyft;

    bindWallPotential<HardWallPotential>(m,"HardWallPotential")
        .def_property_readonly("diameters", py::overload_cast<>(&HardWallPotential::getDiameters), py::return_value_policy::reference_internal);
    }
