#include "_flyft.h"
#include "flyft/boundary_type.h"

#include <pybind11/stl_bind.h>

void bindBoundaryType(py::module_& m)
    {
    using namespace flyft;

    py::bind_map<BoundaryType>(m, "BoundaryType");
    }
