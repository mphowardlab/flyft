#include "_flyft.h"

// function predeclaration in lieu of a bunch of headers
void bindPairMap(py::module_&);
void bindTypeMap(py::module_&);
void bindDoubleParameters(py::module_&);

void bindField(py::module_&);
void bindMesh(py::module_&);
void bindState(py::module_&);

void bindFunctional(py::module_&);
void bindCompositeFunctional(py::module_&);
void bindGrandPotential(py::module_&);
void bindIdealGasFunctional(py::module_&);
void bindRosenfeldFMT(py::module_&);
void bindVirialExpansion(py::module_&);

void bindExternalPotential(py::module_&);
void bindCompositeExternalPotential(py::module_&);
void bindLinearPotential(py::module_&);
void bindWallPotential(py::module_&);
void bindExponentialWallPotential(py::module_&);
void bindHardWallPotential(py::module_&);
void bindHarmonicWallPotential(py::module_&);
void bindLennardJones93WallPotential(py::module_&);

void bindSolver(py::module_&);
void bindPicardIteration(py::module_&);

void bindFlux(py::module_&);
void bindCompositeFlux(py::module_&);
void bindDiffusiveFlux(py::module_&);
void bindBrownianDiffusiveFlux(py::module_&);

void bindIntegrator(py::module_&);
void bindExplicitEulerIntegrator(py::module_&);

PYBIND11_MODULE(_flyft, m)
    {
    bindPairMap(m);
    bindTypeMap(m);
    bindDoubleParameters(m);

    bindField(m);
    bindMesh(m);
    bindState(m);

    bindFunctional(m);
    bindCompositeFunctional(m);
    bindGrandPotential(m);
    bindIdealGasFunctional(m);
    bindRosenfeldFMT(m);
    bindVirialExpansion(m);

    bindExternalPotential(m);
    bindCompositeExternalPotential(m);
    bindLinearPotential(m);
    bindWallPotential(m);
    bindExponentialWallPotential(m);
    bindHardWallPotential(m);
    bindHarmonicWallPotential(m);
    bindLennardJones93WallPotential(m);

    bindSolver(m);
    bindPicardIteration(m);

    bindFlux(m);
    bindCompositeFlux(m);
    bindDiffusiveFlux(m);
    bindBrownianDiffusiveFlux(m);

    bindIntegrator(m);
    bindExplicitEulerIntegrator(m);
    }
