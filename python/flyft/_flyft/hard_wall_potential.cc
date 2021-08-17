#include "_flyft.h"
#include "flyft/hard_wall_potential.h"

void bindHardWallPotential(py::module_& m)
    {
    using namespace flyft;

    py::class_<HardWallPotential,std::shared_ptr<HardWallPotential>,WallPotential>(m, "HardWallPotential")
        .def(py::init<double,double>())
        .def(py::init<std::shared_ptr<DoubleParameter>,double>())
        .def_property_readonly("diameters", py::overload_cast<>(&HardWallPotential::getDiameters), py::return_value_policy::reference_internal)
        ;
    }
