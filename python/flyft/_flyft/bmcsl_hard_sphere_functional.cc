#include "_flyft.h"
#include "flyft/bmcsl_hard_sphere_functional.h"
#include "flyft/functional.h"

void bindBMCSLHardSphereFunctional(py::module_& m)
    {
    using namespace flyft;

    py::class_<BMCSLHardSphereFunctional,std::shared_ptr<BMCSLHardSphereFunctional>,Functional>(m, "BMCSLHardSphereFunctional")
        .def(py::init())
        .def_property("diameters", &BMCSLHardSphereFunctional::getDiameters, &BMCSLHardSphereFunctional::setDiameters, py::return_value_policy::reference_internal)
        ;
    }
