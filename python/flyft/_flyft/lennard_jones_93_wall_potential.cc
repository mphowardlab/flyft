#include "_flyft.h"
#include "flyft/lennard_jones_93_wall_potential.h"

void bindLennardJones93WallPotential(py::module_& m)
    {
    using namespace flyft;

    py::class_<LennardJones93WallPotential,std::shared_ptr<LennardJones93WallPotential>,WallPotential>(m, "LennardJones93WallPotential")
        .def(py::init<double,double>())
        .def_property("epsilons", &LennardJones93WallPotential::getEpsilons, &LennardJones93WallPotential::setEpsilons)
        .def_property("sigmas", &LennardJones93WallPotential::getSigmas, &LennardJones93WallPotential::setSigmas)
        .def_property("cutoffs", &LennardJones93WallPotential::getCutoffs, &LennardJones93WallPotential::setCutoffs)
        .def_property("shifts", &LennardJones93WallPotential::getShifts, &LennardJones93WallPotential::setShifts)
        ;
    }
