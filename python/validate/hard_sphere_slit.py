import numpy as np

import flyft

L = 16.0
diameters = {"A": 1.0}
etas = {"A": 0.4257}


def mu(rho, d):
    eta = rho * np.pi * d**3 / 6.0
    return (
        np.log(rho)
        + (14 * eta - 13 * eta**2 + 5 * eta**3) / (2 * (1 - eta) ** 3)
        - np.log(1 - eta)
    )


# initialize mesh
mesh = flyft.Mesh(L, 1000)
state = flyft.State(mesh, "A")

# setup functionals
ig = flyft.functional.IdealGas()
ig.volumes["A"] = 1.0

fmt = flyft.functional.RosenfeldFMT()
fmt.diameters = diameters

Vext = flyft.external.Composite(
    (flyft.external.HardWall(0.0, 1.0), flyft.external.HardWall(L, -1.0))
)
for p in Vext.potentials:
    p.diameters = diameters

omega = flyft.functional.GrandPotential(ig, fmt, Vext)

# setup initial density based on target bulk packing fraction
x = state.mesh.coordinates
mus = {}
for t in state.fields:
    d = diameters[t]
    inside = np.logical_and(
        x >= Vext.potentials[0].origin + 0.5 * d,
        x < Vext.potentials[1].origin - 0.5 * d,
    )
    rho = (6 * etas[t]) / (np.pi * d**3)
    state.fields[t][inside] = rho
    state.fields[t][~inside] = 0.0

    # make the chemical potentials consistent with the PY chemical potential
    omega.constrain(t, mu(rho, d), omega.Constraint.mu)

# take a short run with a small alpha to make initial changes stable
picard = flyft.solver.PicardIteration(1.0e-3, 1000, 1.0e-12)
conv = picard.solve(omega, state)

# if not converged after this, increase the mixing parameter and iterate longer
if not conv:
    picard.mix_parameter = 1.0e-2
    picard.max_iterations = 10000
    conv = picard.solve(omega, state)
