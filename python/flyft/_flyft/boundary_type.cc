#include "_flyft.h"
#include "flyft/boundary_type.h"

#include <pybind11/stl_bind.h>

void bindBoundaryType(py::module_& m)
    {
    using namespace flyft;
    py::enum_<BoundaryType>(m, "BoundaryType", py::arithmetic())
        .value("periodic", BoundaryType::periodic)
        .value("zero", BoundaryType::zero)
        .value("reflect", BoundaryType::reflect)
        .value("repeat", BoundaryType::repeat)
    }

