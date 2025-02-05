#include "flyft/pair_map.h"

#include "_flyft.h"

#include <pybind11/stl_bind.h>

void bindPairMap(py::module_& m)
    {
    using namespace flyft;

    py::bind_map<PairMap<double>>(m, "PairMapDouble");
    }
