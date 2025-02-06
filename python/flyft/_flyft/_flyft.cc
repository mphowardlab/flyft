#include "_flyft.h"

// function predeclaration in lieu of a bunch of headers
void bindTrackedObject(py::module_&);
void bindPairMap(py::module_&);
void bindTypeMap(py::module_&);
void bindVector(py::module_&);
void bindDoubleParameters(py::module_&);

void bindBoundaryType(py::module_&);
void bindCommunicator(py::module_&);
void bindField(py::module_&);
void bindMesh(py::module_&);
void bindSphericalMesh(py::module_&);
void bindCartesianMesh(py::module_&);
void bindParallelMesh(py::module_&);
void bindState(py::module_&);

void bindFunctional(py::module_&);
void bindBoublikHardSphereFunctional(py::module_&);
void bindCompositeFunctional(py::module_&);
void bindGrandPotential(py::module_&);
void bindIdealGasFunctional(py::module_&);
void bindRosenfeldFMT(py::module_&);
void bindVirialExpansion(py::module_&);
void bindWhiteBear(py::module_&);
void bindWhiteBearMarkII(py::module_&);

void bindExternalPotential(py::module_&);
void bindCompositeExternalPotential(py::module_&);
void bindLinearPotential(py::module_&);
void bindExponentialWallPotential(py::module_&);
void bindHardWallPotential(py::module_&);
void bindHarmonicWallPotential(py::module_&);
void bindLennardJones93WallPotential(py::module_&);

void bindSolver(py::module_&);
void bindPicardIteration(py::module_&);

void bindFlux(py::module_&);
void bindCompositeFlux(py::module_&);
void bindBrownianDiffusiveFlux(py::module_&);
void bindRPYDiffusiveFlux(py::module_&);

void bindIntegrator(py::module_&);
void bindCrankNicolsonIntegrator(py::module_&);
void bindExplicitEulerIntegrator(py::module_&);
void bindImplicitEulerIntegrator(py::module_&);

#ifdef FLYFT_MPI
#include <mpi.h>
#endif

PYBIND11_MODULE(_flyft, m)
    {
#ifdef FLYFT_MPI
    int mpi_init = 0;
    MPI_Initialized(&mpi_init);
    if (!mpi_init)
        {
        MPI_Init(NULL, NULL);
        Py_AtExit([]() { MPI_Finalize(); });
        }
#endif

    bindTrackedObject(m);
    bindPairMap(m);
    bindTypeMap(m);
    bindVector(m);
    bindDoubleParameters(m);

    bindBoundaryType(m);
    bindCommunicator(m);
    bindField(m);
    bindMesh(m);
    bindCartesianMesh(m);
    bindSphericalMesh(m);
    bindParallelMesh(m);
    bindState(m);

    bindFunctional(m);
    bindBoublikHardSphereFunctional(m);
    bindCompositeFunctional(m);
    bindGrandPotential(m);
    bindIdealGasFunctional(m);
    bindRosenfeldFMT(m);
    bindVirialExpansion(m);
    bindWhiteBear(m);
    bindWhiteBearMarkII(m);

    bindExternalPotential(m);
    bindCompositeExternalPotential(m);
    bindLinearPotential(m);
    bindExponentialWallPotential(m);
    bindHardWallPotential(m);
    bindHarmonicWallPotential(m);
    bindLennardJones93WallPotential(m);

    bindSolver(m);
    bindPicardIteration(m);

    bindFlux(m);
    bindCompositeFlux(m);
    bindBrownianDiffusiveFlux(m);
    bindRPYDiffusiveFlux(m);

    bindIntegrator(m);
    bindCrankNicolsonIntegrator(m);
    bindExplicitEulerIntegrator(m);
    bindImplicitEulerIntegrator(m);
    }
