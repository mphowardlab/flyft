#include "_flyft.h"
#include "flyft/state.h"

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

PYBIND11_MAKE_OPAQUE(std::vector<std::shared_ptr<flyft::Field>>);

void bindState(py::module_& m)
    {
    using namespace flyft;

    py::class_<State,std::shared_ptr<State>>(m, "State")
        .def(py::init<std::shared_ptr<const Mesh>,int>())
        .def_property_readonly("mesh", &State::getMesh)
        .def_property_readonly("num_fields", &State::getNumFields)
        .def("getField", py::overload_cast<int>(&State::getField))
        .def_property_readonly("field", &State::getFields)
        .def_property("diameter",
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
        .def_property("ideal_volume",
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
        ;
    }
