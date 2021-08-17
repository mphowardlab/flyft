#include "_flyft.h"
#include "flyft/functional.h"
#include "flyft/composite_functional.h"

void bindGrandPotential(py::module_& m)
    {
    using namespace flyft;

    py::class_<GrandPotential,std::shared_ptr<GrandPotential>,Functional> grand(m, "GrandPotential");
    grand.def(py::init())
        .def_property("ideal",  py::overload_cast<>(&GrandPotential::getIdealGasFunctional), &GrandPotential::setIdealGasFunctional)
        .def_property("excess", py::overload_cast<>(&GrandPotential::getExcessFunctional), &GrandPotential::setExcessFunctional)
        .def_property("external", py::overload_cast<>(&GrandPotential::getExternalPotential), &GrandPotential::setExternalPotential)
        .def_property_readonly("constraints", py::overload_cast<>(&GrandPotential::getConstraints), py::return_value_policy::reference_internal)
        .def_property_readonly("constraint_types", py::overload_cast<>(&GrandPotential::getConstraintTypes), py::return_value_policy::reference_internal)
        ;

    py::enum_<GrandPotential::Constraint>(grand, "Constraint", py::arithmetic())
        .value("N", GrandPotential::Constraint::N)
        .value("mu", GrandPotential::Constraint::mu)
        ;
    }
