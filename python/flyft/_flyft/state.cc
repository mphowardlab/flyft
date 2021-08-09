#include "_flyft.h"
#include "flyft/state.h"

#include <pybind11/stl.h>

void bindState(py::module_& m)
    {
    using namespace flyft;

    py::class_<State,std::shared_ptr<State>> state(m, "State");
    state.def(py::init<std::shared_ptr<Mesh>,const std::string&>())
        .def(py::init<std::shared_ptr<Mesh>,const std::vector<std::string>&>())
        .def_property_readonly("mesh", [](const State& self){ return self.getMesh()->global(); })
        .def_property_readonly("num_fields", &State::getNumFields)
        .def_property_readonly("fields", &State::getFields, py::return_value_policy::reference_internal)
        .def_property("time", &State::getTime, &State::setTime)
        ;
    }
