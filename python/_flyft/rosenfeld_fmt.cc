#include "_flyft.h"
#include "flyft/free_energy_functional.h"
#include "flyft/rosenfeld_fmt.h"

void bindRosenfeldFMT(py::module_& m)
    {
    using namespace flyft;

    py::class_<RosenfeldFMT,std::shared_ptr<RosenfeldFMT>,FreeEnergyFunctional>(m, "RosenfeldFMT")
        .def(py::init())
        ;
    }
