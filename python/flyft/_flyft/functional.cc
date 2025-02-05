#include "flyft/functional.h"

#include "_flyft.h"

#include <pybind11/stl.h>

namespace flyft
    {
//! Trampoline for Functional to python
class FunctionalTrampoline : public Functional
    {
    public:
    // Pull in constructors
    using Functional::Functional;

    protected:
    //! pybind11 override of pure virtual compute method
    void _compute(std::shared_ptr<State> state, bool compute_value) override
        {
        PYBIND11_OVERRIDE_PURE(void, Functional, _compute, state, compute_value);
        }
    };
    } // namespace flyft

void bindFunctional(py::module_& m)
    {
    using namespace flyft;

    py::class_<Functional, std::shared_ptr<Functional>, FunctionalTrampoline>(m, "Functional")
        .def(py::init<>())
        .def("compute", &Functional::compute)
        .def_property_readonly("value", &Functional::getValue)
        .def_property_readonly("derivatives",
                               &Functional::getDerivatives,
                               py::return_value_policy::reference_internal);
    }
