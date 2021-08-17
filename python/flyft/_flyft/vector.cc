#include "_flyft.h"

#include <pybind11/stl_bind.h>

void bindVector(py::module& m)
    {
    using namespace flyft;

    py::bind_vector<std::vector<std::string>>(m, "VectorString");
    py::bind_vector<std::vector<std::shared_ptr<Flux>>>(m,"VectorFlux");
    py::bind_vector<std::vector<std::shared_ptr<Functional>>>(m, "VectorFunctional");
    py::bind_vector<std::vector<std::shared_ptr<ExternalPotential>>>(m, "VectorExternalPotential");
    }
