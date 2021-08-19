#include "_flyft.h"
#include "flyft/type_map.h"

#include <pybind11/stl_bind.h>

template<class T>
void bind_type_map(py::module& m, const std::string& name)
    {
    using Map = flyft::TypeMap<T>;
    py::bind_map<Map>(m,name.c_str());
    }

void bindTypeMap(py::module& m)
    {
    using namespace flyft;

    bind_type_map<bool>(m, "TypeMapBool");
    bind_type_map<double>(m, "TypeMapDouble");
    bind_type_map<std::shared_ptr<Field>>(m, "TypeMapField");
    bind_type_map<GrandPotential::Constraint>(m, "TypeMapConstraint");
    }
