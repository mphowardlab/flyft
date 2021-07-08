#include "_flyft.h"
#include "flyft/integrator.h"

#include <pybind11/stl.h>

namespace flyft
{
class IntegratorTrampoline : public Integrator
    {
    public:
        using Integrator::Integrator;

        void step(std::shared_ptr<Flux> flux,
                     std::shared_ptr<GrandPotential> grand,
                     std::shared_ptr<State> state,
                     double timestep) override
            {
            PYBIND11_OVERRIDE_PURE(void, Integrator, advance, flux, grand, state, timestep);
            }
    };
}

void bindIntegrator(py::module_& m)
    {
    using namespace flyft;

    py::class_<Integrator,std::shared_ptr<Integrator>,IntegratorTrampoline>(m, "Integrator")
        .def(py::init<double>())
        .def("advance", &Integrator::advance)
        .def_property("timestep", &Integrator::getTimestep, &Integrator::setTimestep)
        ;
    }
