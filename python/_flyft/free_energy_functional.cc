#include "_flyft.h"
#include "flyft/free_energy_functional.h"

namespace flyft
{
//! Trampoline for FreeEnergyFunctional to python
class FreeEnergyFunctionalTrampoline : public FreeEnergyFunctional
    {
    public:
        // Pull in constructors
        using FreeEnergyFunctional::FreeEnergyFunctional;

        //! pybind11 override of pure virtual compute method
        void compute(std::shared_ptr<State> state) override
            {
            PYBIND11_OVERRIDE_PURE(void, FreeEnergyFunctional, compute, state);
            }
    };
}

void bindFreeEnergyFunctional(py::module_& m)
    {
    using namespace flyft;

    py::class_<FreeEnergyFunctional,std::shared_ptr<FreeEnergyFunctional>,FreeEnergyFunctionalTrampoline>(m, "FreeEnergyFunctional")
        .def(py::init<>())
        .def("compute", &FreeEnergyFunctional::compute)
        .def_property_readonly("value", &FreeEnergyFunctional::getValue)
        .def_property_readonly("derivative", &FreeEnergyFunctional::getDerivatives)
        ;
    }
