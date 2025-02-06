#include "flyft/explicit_euler_integrator.h"

#include "_flyft.h"

void bindExplicitEulerIntegrator(py::module_& m)
    {
    using namespace flyft;

    py::class_<ExplicitEulerIntegrator, std::shared_ptr<ExplicitEulerIntegrator>, Integrator>(
        m,
        "ExplicitEulerIntegrator")
        .def(py::init<double>());
    }
