#include "_flyft.h"

// function predeclaration in lieu of a bunch of headers
void bindField(py::module_&);
void bindMesh(py::module_&);
void bindState(py::module_&);

void bindFreeEnergyFunctional(py::module_&);
void bindCompositeFunctional(py::module_&);
void bindIdealGasFunctional(py::module_&);
void bindRosenfeldFMT(py::module_&);

void bindExternalPotential(py::module_&);
void bindHardWallPotential(py::module_&);

void bindSolver(py::module_&);
void bindPiccardIteration(py::module_&);

PYBIND11_MODULE(_flyft, m)
    {
    bindField(m);
    bindMesh(m);
    bindState(m);

    bindFreeEnergyFunctional(m);
    bindCompositeFunctional(m);
    bindIdealGasFunctional(m);
    bindRosenfeldFMT(m);

    bindExternalPotential(m);
    bindHardWallPotential(m);

    bindSolver(m);
    bindPiccardIteration(m);
    }
