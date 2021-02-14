#include "_flyft.h"
#include "flyft/type_map.h"

#include <pybind11/stl_bind.h>

void bindTypeMap(py::module& m)
    {
    using namespace flyft;

    py::bind_map<TypeMap<double>>(m, "TypeMapDouble");
    py::bind_map<TypeMap<Field>>(m, "TypeMapField");

    py::bind_vector<std::vector<std::string>>(m, "VectorString");
    }
