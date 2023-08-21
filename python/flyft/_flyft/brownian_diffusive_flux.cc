#include "_flyft.h"
#include "flyft/brownian_diffusive_flux.h"

#include <pybind11/stl.h>

namespace flyft
{
//! Trampoline for DiffusiveFlux to python
class BrownianDiffusiveFluxTrampoline : public BrownianDiffusiveFlux
    {
    public:
        // Pull in constructors
        using BrownianDiffusiveFlux::BrownianDiffusiveFlux;

        //! pybind11 override of pure virtual compute method
        void compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state) override
            {
            PYBIND11_OVERRIDE_PURE(void, BrownianDiffusiveFlux, compute, grand, state);
            }
    };
}

void bindBrownianDiffusiveFlux(py::module_& m)
    {
    using namespace flyft;

    py::class_<BrownianDiffusiveFlux,std::shared_ptr<BrownianDiffusiveFlux>,Flux>(m, "BrownianDiffusiveFlux")
        .def(py::init<>())
        .def_property_readonly("diffusivities", py::overload_cast<>(&BrownianDiffusiveFlux::getDiffusivities), py::return_value_policy::reference_internal)
        ;
    }
