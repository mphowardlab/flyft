#include "_flyft.h"
#include "flyft/free_energy_functional.h"
#include "flyft/ideal_gas_functional.h"

void bindIdealGasFunctional(py::module_& m)
    {
    using namespace flyft;

    py::class_<IdealGasFunctional,std::shared_ptr<IdealGasFunctional>,FreeEnergyFunctional>(m, "IdealGasFunctional")
        .def(py::init())
        ;
    }