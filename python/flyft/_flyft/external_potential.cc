#include "flyft/external_potential.h"

#include "_flyft.h"

namespace flyft
    {
//! Trampoline for ExternalPotential to python
class ExternalPotentialTrampoline : public ExternalPotential
    {
    public:
    // Pull in constructors
    using ExternalPotential::ExternalPotential;

    protected:
    //! pybind11 override of pure virtual compute method
    void computePotentials(std::shared_ptr<State> state) override
        {
        PYBIND11_OVERRIDE_PURE(void, ExternalPotential, computePotentials, state);
        }
    };
    } // namespace flyft

void bindExternalPotential(py::module_& m)
    {
    using namespace flyft;

    py::class_<ExternalPotential,
               std::shared_ptr<ExternalPotential>,
               ExternalPotentialTrampoline,
               Functional>(m, "ExternalPotential")
        .def(py::init<>());
    }
