#include "_flyft.h"
#include "flyft/diffusive_flux.h"

#include <pybind11/stl.h>

namespace flyft
{
//! Trampoline for DiffusiveFlux to python
class DiffusiveFluxTrampoline : public DiffusiveFlux
    {
    public:
        // Pull in constructors
        using DiffusiveFlux::DiffusiveFlux;

        //! pybind11 override of pure virtual compute method
        void compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state) override
            {
            PYBIND11_OVERRIDE_PURE(void, DiffusiveFlux, compute, grand, state);
            }
    };
}

void bindDiffusiveFlux(py::module_& m)
    {
    using namespace flyft;

    py::class_<DiffusiveFlux,std::shared_ptr<DiffusiveFlux>,DiffusiveFluxTrampoline,Flux>(m, "DiffusiveFlux")
        .def(py::init<>())
        .def_property_readonly("diffusivities", py::overload_cast<>(&DiffusiveFlux::getDiffusivities), py::return_value_policy::reference_internal)
        ;
    }
