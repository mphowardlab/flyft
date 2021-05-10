#include "_flyft.h"
#include "flyft/exponential_wall_potential.h"

void bindExponentialWallPotential(py::module_& m)
    {
    using namespace flyft;

    py::class_<ExponentialWallPotential,std::shared_ptr<ExponentialWallPotential>,WallPotential>(m, "ExponentialWallPotential")
        .def(py::init<double,double>())
        .def(py::init<std::shared_ptr<DoubleParameter>,double>())
        .def_property("epsilons", &ExponentialWallPotential::getEpsilons, &ExponentialWallPotential::setEpsilons)
        .def_property("kappas", &ExponentialWallPotential::getKappas, &ExponentialWallPotential::setKappas)
        .def_property("shifts", &ExponentialWallPotential::getShifts, &ExponentialWallPotential::setShifts)
        ;
    }
