#include "_flyft.h"
#include "flyft/implicit_euler_integrator.h"

void bindImplicitEulerIntegrator(py::module_& m)
    {
    using namespace flyft;

    py::class_<ImplicitEulerIntegrator,std::shared_ptr<ImplicitEulerIntegrator>,Integrator>(m, "ImplicitEulerIntegrator")
        .def(py::init<double,double,int,double>())
        .def_property("mix_parameter", &ImplicitEulerIntegrator::getMixParameter, &ImplicitEulerIntegrator::setMixParameter)
        .def_property("max_iterations", &ImplicitEulerIntegrator::getMaxIterations, &ImplicitEulerIntegrator::setMaxIterations)
        .def_property("tolerance", &ImplicitEulerIntegrator::getTolerance, &ImplicitEulerIntegrator::setTolerance)
        ;
    }
