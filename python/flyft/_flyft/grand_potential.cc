#include "_flyft.h"
#include "flyft/free_energy_functional.h"
#include "flyft/composite_functional.h"

void bindGrandPotential(py::module_& m)
    {
    using namespace flyft;

    py::class_<GrandPotential,std::shared_ptr<GrandPotential>,FreeEnergyFunctional> grand(m, "GrandPotential");
    grand.def(py::init())
        .def_property_readonly("ideal", &GrandPotential::getIdealGasFunctional)
        .def_property_readonly("intrinsic", &GrandPotential::getIntrinsicFunctional)
        .def_property_readonly("excess", &GrandPotential::getExcessIntrinsicFunctional)
        .def_property_readonly("external", &GrandPotential::getExternalPotential)
        .def_property("constraints", &GrandPotential::getConstraints, &GrandPotential::setConstraints, py::return_value_policy::reference_internal)
        .def_property("constraint_types", &GrandPotential::getConstraintTypes, &GrandPotential::setConstraintTypes, py::return_value_policy::reference_internal)
        ;

    py::enum_<GrandPotential::Constraint>(grand, "Constraint", py::arithmetic())
        .value("N", GrandPotential::Constraint::N)
        .value("mu", GrandPotential::Constraint::mu)
        ;
    }
