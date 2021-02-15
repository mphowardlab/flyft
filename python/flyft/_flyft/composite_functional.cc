#include "_flyft.h"
#include "flyft/free_energy_functional.h"
#include "flyft/composite_functional.h"

void bindCompositeFunctional(py::module_& m)
    {
    using namespace flyft;

    py::class_<CompositeFunctional,std::shared_ptr<CompositeFunctional>,FreeEnergyFunctional>(m, "CompositeFunctional")
        .def(py::init())
        .def(py::init<std::shared_ptr<FreeEnergyFunctional>>())
        .def(py::init(
            [](py::iterable functionals) {
                auto comp_f = std::make_shared<CompositeFunctional>();
                for (auto& f : functionals)
                    {
                    comp_f->addFunctional(py::cast<std::shared_ptr<FreeEnergyFunctional>>(f));
                    }
                return comp_f;
            }))
        .def("append", &CompositeFunctional::addFunctional)
        .def("extend",
            [](std::shared_ptr<CompositeFunctional> self, py::iterable functionals) {
                for (auto& f : functionals)
                    {
                    self->addFunctional(py::cast<std::shared_ptr<FreeEnergyFunctional>>(f));
                    }
            })
        .def("remove", &CompositeFunctional::removeFunctional)
        .def("remove",
            [](std::shared_ptr<CompositeFunctional> self, py::iterable functionals) {
                for (auto& f : functionals)
                    {
                    self->removeFunctional(py::cast<std::shared_ptr<FreeEnergyFunctional>>(f));
                    }
            })
        ;
    }
