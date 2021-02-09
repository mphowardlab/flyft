#include "_flyft.h"

// function predeclaration in lieu of a bunch of headers
void bindField(py::module_&);
void bindMesh(py::module_&);
void bindState(py::module_&);

PYBIND11_MODULE(_flyft, m)
    {
    bindField(m);
    bindMesh(m);
    bindState(m);
    }
