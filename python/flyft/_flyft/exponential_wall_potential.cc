#include "_flyft.h"
#include "flyft/exponential_wall_potential.h"

void bindExponentialWallPotential(py::module_& m)
    {
    using namespace flyft;

    py::class_<ExponentialWallPotential,std::shared_ptr<ExponentialWallPotential>,WallPotential>(m, "ExponentialWallPotential")
        .def(py::init<double,double>())
        .def(py::init<std::shared_ptr<DoubleParameter>,double>())
        .def_property_readonly("epsilons", py::overload_cast<>(&ExponentialWallPotential::getEpsilons))
        .def_property_readonly("kappas", py::overload_cast<>(&ExponentialWallPotential::getKappas))
        .def_property_readonly("shifts", py::overload_cast<>(&ExponentialWallPotential::getShifts))
        ;
    }
