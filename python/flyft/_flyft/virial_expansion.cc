#include "_flyft.h"
#include "flyft/functional.h"
#include "flyft/virial_expansion.h"

void bindVirialExpansion(py::module_& m)
    {
    using namespace flyft;

    py::class_<VirialExpansion,std::shared_ptr<VirialExpansion>,Functional>(m, "VirialExpansion")
        .def(py::init())
        .def_property_readonly("coefficients", py::overload_cast<>(&VirialExpansion::getCoefficients), py::return_value_policy::reference_internal)
        ;
    }
