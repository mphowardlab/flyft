#include "_flyft.h"
#include "flyft/integrator.h"

#include <pybind11/stl.h>

namespace flyft
{
class IntegratorTrampoline : public Integrator
    {
    public:
        using Integrator::Integrator;

        bool advance(std::shared_ptr<Flux> flux,
                     std::shared_ptr<GrandPotential> grand,
                     std::shared_ptr<State> state,
                     double time) override
            {
            PYBIND11_OVERRIDE_PURE(bool, Integrator, advance, flux, grand, state, time);
            }
    };
}

void bindIntegrator(py::module_& m)
    {
    using namespace flyft;

    py::class_<Integrator,std::shared_ptr<Integrator>,IntegratorTrampoline>(m, "Integrator")
        .def(py::init<>())
        .def("advance", &Integrator::advance)
        ;
    }
