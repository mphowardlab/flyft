#include "flyft/boundary_type.h"

#include "_flyft.h"

void bindBoundaryType(py::module_& m)
    {
    using namespace flyft;
    py::enum_<BoundaryType>(m, "BoundaryType")
        .value("periodic", BoundaryType::periodic)
        .value("zero", BoundaryType::zero)
        .value("reflect", BoundaryType::reflect)
        .value("repeat", BoundaryType::repeat);
    }
