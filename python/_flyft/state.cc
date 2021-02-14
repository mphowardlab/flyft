#include "_flyft.h"
#include "flyft/state.h"

#include <pybind11/stl.h>

void bindState(py::module_& m)
    {
    using namespace flyft;

    py::class_<State,std::shared_ptr<State>> state(m, "State");
    state.def(py::init<std::shared_ptr<const Mesh>,const std::string&>())
        .def(py::init<std::shared_ptr<const Mesh>,const std::vector<std::string>&>())
        .def_property_readonly("mesh", &State::getMesh)
        .def_property_readonly("num_fields", &State::getNumFields)
        .def_property_readonly("fields", &State::getFields)
        .def_property("diameters", &State::getDiameters, &State::setDiameters)
        .def_property("ideal_volumes", &State::getIdealVolumes, &State::setIdealVolumes)
        .def("setConstraint", &State::setConstraint)
        ;

    py::enum_<State::Constraint>(state, "Constraint", py::arithmetic())
        .value("compute", State::Constraint::compute)
        .value("N", State::Constraint::N)
        .value("mu", State::Constraint::mu)
        ;
    }
