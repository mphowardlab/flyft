#include "_flyft.h"
#include "flyft/hard_wall_potential.h"

void bindHardWallPotential(py::module_& m)
    {
    using namespace flyft;

    py::class_<HardWallPotential,std::shared_ptr<HardWallPotential>,WallPotential>(m, "HardWallPotential")
        .def(py::init<double,double>())
        .def_property("diameters", &HardWallPotential::getDiameters, &HardWallPotential::setDiameters, py::return_value_policy::reference_internal)
        ;
    }
