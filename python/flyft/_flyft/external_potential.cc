#include "_flyft.h"
#include "flyft/external_potential.h"

namespace flyft
{
//! Trampoline for ExternalPotential to python
class ExternalPotentialTrampoline : public ExternalPotential
    {
    public:
        // Pull in constructors
        using ExternalPotential::ExternalPotential;

        //! pybind11 override of pure virtual compute method
        void potential(std::shared_ptr<Field> V,
                       const std::string& type,
                       std::shared_ptr<State> state) override
            {
            PYBIND11_OVERRIDE_PURE(void, ExternalPotential, potential, V, type, state);
            }
    };
}

void bindExternalPotential(py::module_& m)
    {
    using namespace flyft;

    py::class_<ExternalPotential,std::shared_ptr<ExternalPotential>,ExternalPotentialTrampoline,FreeEnergyFunctional>(m, "ExternalPotential")
        .def(py::init<>())
        ;
    }
