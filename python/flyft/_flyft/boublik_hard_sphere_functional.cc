#include "_flyft.h"
#include "flyft/boublik_hard_sphere_functional.h"
#include "flyft/functional.h"

void bindBoublikHardSphereFunctional(py::module_& m)
    {
    using namespace flyft;

    py::class_<BoublikHardSphereFunctional,std::shared_ptr<BoublikHardSphereFunctional>,Functional>(m, "BoublikHardSphereFunctional")
        .def(py::init())
        .def_property_readonly("diameters", py::overload_cast<>(&BoublikHardSphereFunctional::diameters), py::return_value_policy::reference_internal)
        ;
    }
