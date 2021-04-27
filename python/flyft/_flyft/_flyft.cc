#include "_flyft.h"

// function predeclaration in lieu of a bunch of headers
void bindTypeMap(py::module_&);

void bindField(py::module_&);
void bindMesh(py::module_&);
void bindState(py::module_&);

void bindFunctional(py::module_&);
void bindCompositeFunctional(py::module_&);
void bindGrandPotential(py::module_&);
void bindIdealGasFunctional(py::module_&);
void bindRosenfeldFMT(py::module_&);

void bindExternalPotential(py::module_&);
void bindCompositeExternalPotential(py::module_&);
void bindWallPotential(py::module_&);
void bindExponentialWallPotential(py::module_&);
void bindHardWallPotential(py::module_&);
void bindHarmonicWallPotential(py::module_&);
void bindLennardJones93WallPotential(py::module_&);

void bindSolver(py::module_&);
void bindPicardIteration(py::module_&);

void bindFlux(py::module_&);
void bindDiffusiveFlux(py::module_& m);
void bindBrownianDiffusiveFlux(py::module_& m);

PYBIND11_MODULE(_flyft, m)
    {
    bindTypeMap(m);

    bindField(m);
    bindMesh(m);
    bindState(m);

    bindFunctional(m);
    bindCompositeFunctional(m);
    bindGrandPotential(m);
    bindIdealGasFunctional(m);
    bindRosenfeldFMT(m);

    bindExternalPotential(m);
    bindCompositeExternalPotential(m);
    bindWallPotential(m);
    bindExponentialWallPotential(m);
    bindHardWallPotential(m);
    bindHarmonicWallPotential(m);
    bindLennardJones93WallPotential(m);

    bindSolver(m);
    bindPicardIteration(m);

    bindFlux(m);
    bindDiffusiveFlux(m);
    bindBrownianDiffusiveFlux(m);
    }
