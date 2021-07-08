#include "_flyft.h"
#include "flyft/crank_nicolson_integrator.h"

void bindCrankNicolsonIntegrator(py::module_& m)
    {
    using namespace flyft;

    py::class_<CrankNicolsonIntegrator,std::shared_ptr<CrankNicolsonIntegrator>,Integrator>(m, "CrankNicolsonIntegrator")
        .def(py::init<double,double,int,double>())
        .def_property("mix_parameter", &CrankNicolsonIntegrator::getMixParameter, &CrankNicolsonIntegrator::setMixParameter)
        .def_property("max_iterations", &CrankNicolsonIntegrator::getMaxIterations, &CrankNicolsonIntegrator::setMaxIterations)
        .def_property("tolerance", &CrankNicolsonIntegrator::getTolerance, &CrankNicolsonIntegrator::setTolerance)
        ;
    }
