#include "_flyft.h"
#include "flyft/functional.h"
#include "flyft/ideal_gas_functional.h"

void bindIdealGasFunctional(py::module_& m)
    {
    using namespace flyft;

    py::class_<IdealGasFunctional,std::shared_ptr<IdealGasFunctional>,Functional>(m, "IdealGasFunctional")
        .def(py::init())
        .def_property_readonly("volumes", py::overload_cast<>(&IdealGasFunctional::getVolumes), py::return_value_policy::reference_internal)
        ;
    }
