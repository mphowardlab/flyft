#include "_flyft.h"
#include "flyft/free_energy_functional.h"
#include "flyft/composite_functional.h"

void bindGrandPotential(py::module_& m)
    {
    using namespace flyft;

    py::class_<GrandPotential,std::shared_ptr<GrandPotential>,FreeEnergyFunctional> grand(m, "GrandPotential");
    grand.def(py::init())
        .def_property("ideal",  py::overload_cast<>(&GrandPotential::getIdealGasFunctional), &GrandPotential::setIdealGasFunctional)
        .def_property("excess", py::overload_cast<>(&GrandPotential::getExcessFunctional), &GrandPotential::setExcessFunctional)
        .def_property("external", py::overload_cast<>(&GrandPotential::getExternalPotential), &GrandPotential::setExternalPotential)
        .def_property("constraints", &GrandPotential::getConstraints, &GrandPotential::setConstraints, py::return_value_policy::reference_internal)
        .def_property("constraint_types", &GrandPotential::getConstraintTypes, &GrandPotential::setConstraintTypes, py::return_value_policy::reference_internal)
        ;

    py::enum_<GrandPotential::Constraint>(grand, "Constraint", py::arithmetic())
        .value("N", GrandPotential::Constraint::N)
        .value("mu", GrandPotential::Constraint::mu)
        ;
    }
