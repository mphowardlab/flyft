#include "_flyft.h"
#include "flyft/state.h"

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

void bindState(py::module_& m)
    {
    using namespace flyft;

    py::class_<State,std::shared_ptr<State>> state(m, "State");
    state.def(py::init<std::shared_ptr<const Mesh>,int>())
        .def_property_readonly("mesh", &State::getMesh)
        .def_property_readonly("num_fields", &State::getNumFields)
        .def_property_readonly("fields",
            [](State& state) -> py::tuple {
                py::list res;
                for (auto& f : state.getFields())
                    {
                    res.append(f);
                    }
                return py::tuple(res);
            })
        .def_property("diameters",
            [](py::object self) -> py::array {
                auto _self = self.cast<State*>();
                return py::array(_self->getNumFields(),
                                 _self->getDiameters().data(),
                                 self);
            },
            [](py::object self, py::list diameters) {
                auto _self = py::cast<State*>(self);
                if (static_cast<int>(diameters.size()) != _self->getNumFields())
                    {
                    // error: wrong shape
                    }
                for (int i=0; i < _self->getNumFields(); ++i)
                    {
                    _self->setDiameter(i,py::cast<double>(diameters[i]));
                    }
            })
        .def_property("ideal_volumes",
            [](py::object self) -> py::array {
                auto _self = self.cast<State*>();
                return py::array(_self->getNumFields(),
                                 _self->getIdealVolumes().data(),
                                 self);
            },
            [](py::object self, py::list ideal_volumes) {
                auto _self = py::cast<State*>(self);
                if (static_cast<int>(ideal_volumes.size()) != _self->getNumFields())
                    {
                    // error: wrong shape
                    }
                for (int i=0; i < _self->getNumFields(); ++i)
                    {
                    _self->setIdealVolume(i,py::cast<double>(ideal_volumes[i]));
                    }
            })
        .def("setConstraint", &State::setConstraint)
        ;

    py::enum_<State::Constraint>(state, "Constraint", py::arithmetic())
        .value("compute", State::Constraint::compute)
        .value("N", State::Constraint::N)
        .value("mu", State::Constraint::mu)
        ;
    }
