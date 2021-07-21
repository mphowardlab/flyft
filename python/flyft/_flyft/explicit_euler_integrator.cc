#include "_flyft.h"
#include "flyft/explicit_euler_integrator.h"

void bindExplicitEulerIntegrator(py::module_& m)
    {
    using namespace flyft;

    py::class_<ExplicitEulerIntegrator,std::shared_ptr<ExplicitEulerIntegrator>,Integrator>(m, "ExplicitEulerIntegrator")
        .def(py::init<double>())
        ;
    }
