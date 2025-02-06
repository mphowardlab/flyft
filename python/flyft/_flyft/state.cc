#include "flyft/state.h"

#include "_flyft.h"

#include <pybind11/stl.h>

void bindState(py::module_& m)
    {
    using namespace flyft;

    py::class_<State, std::shared_ptr<State>>(m, "State")
        .def(py::init<std::shared_ptr<ParallelMesh>, const std::string&>())
        .def(py::init<std::shared_ptr<ParallelMesh>, const std::vector<std::string>&>())
        .def_property_readonly("communicator", py::overload_cast<>(&State::getCommunicator))
        .def_property_readonly("mesh", py::overload_cast<>(&State::getMesh, py::const_))
        .def_property_readonly("num_fields", &State::getNumFields)
        .def_property_readonly("fields",
                               &State::getFields,
                               py::return_value_policy::reference_internal)
        .def_property("time", &State::getTime, &State::setTime)
        .def("gather_field", &State::gatherField);
    }
