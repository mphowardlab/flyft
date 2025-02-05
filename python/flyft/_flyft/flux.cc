#include "flyft/flux.h"

#include "_flyft.h"

#include <pybind11/stl.h>

namespace flyft
    {
//! Trampoline for Flux to python
class FluxTrampoline : public Flux
    {
    public:
    // Pull in constructors
    using Flux::Flux;

    //! pybind11 override of pure virtual compute method
    void compute(std::shared_ptr<GrandPotential> grand, std::shared_ptr<State> state) override
        {
        PYBIND11_OVERRIDE_PURE(void, Flux, compute, grand, state);
        }
    };
    } // namespace flyft

void bindFlux(py::module_& m)
    {
    using namespace flyft;

    py::class_<Flux, std::shared_ptr<Flux>, FluxTrampoline>(m, "Flux")
        .def(py::init<>())
        .def("compute", &Flux::compute)
        .def_property_readonly("fluxes",
                               &Flux::getFluxes,
                               py::return_value_policy::reference_internal);
    }
