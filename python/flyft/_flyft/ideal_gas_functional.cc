#include "_flyft.h"
#include "flyft/functional.h"
#include "flyft/ideal_gas_functional.h"

void bindIdealGasFunctional(py::module_& m)
    {
    using namespace flyft;

    py::class_<IdealGasFunctional,std::shared_ptr<IdealGasFunctional>,Functional>(m, "IdealGasFunctional")
        .def(py::init())
        .def_property("volumes", &IdealGasFunctional::getVolumes, &IdealGasFunctional::setVolumes, py::return_value_policy::reference_internal)
        ;
    }
