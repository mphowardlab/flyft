import numpy as np

import flyft

L = 10.0
dx = 0.05
diameters = {"A": 1.0}
etas = {"A": 0.1}
detas = {"A": 0.1}
Ds = {"A": 1.0}

# initialize mesh
padding = 0.5 * max(diameters.values())
Lpad = L + 2 * padding
mesh = flyft.Mesh(Lpad, int(Lpad / dx))
state = flyft.State(mesh, tuple(diameters.keys()))

# setup functionals
ig = flyft.functional.IdealGas()
for t in state.fields:
    ig.volumes[t] = 1.0

fmt = flyft.functional.RosenfeldFMT()
fmt.diameters = diameters

Vext = flyft.external.CompositeExternalPotential(
    (
        flyft.external.HardWall(padding, 1.0),
        flyft.external.HardWall(mesh.L - padding, -1.0),
    )
)
for p in Vext.objects:
    p.diameters = diameters

omega = flyft.functional.GrandPotential(ig, fmt, Vext)

# setup initial density based on target packing-fraction profiles
x = state.mesh.coordinates
for t in state.fields:
    d = diameters[t]
    rho = (6 * etas[t]) / (np.pi * d**3)
    drho = (6 * detas[t]) / (np.pi * d**3)

    inside = np.logical_and(
        x >= Vext[0].origin + 0.5 * d, x <= Vext[1].origin - 0.5 * d
    )
    Linside = Vext[1].origin - Vext[0].origin - d
    state.fields[t][inside] = (rho - 0.5 * drho) + (drho / Linside) * (
        x[inside] - Vext[0].origin + 0.5 * d
    )
    state.fields[t][~inside] = 0.0

    N = np.sum(state.fields[t]) * mesh.step
    omega.constrain(t, N, omega.Constraint.N)

# Euler integration of density profile
euler = flyft.dynamics.ExplicitEulerIntegrator(timestep=1.0e-3)
flux = flyft.dynamics.BrownianDiffusiveFlux()
flux.diffusivities = Ds

# perform integration
t_sample = 5.0
num_sample = 5
traj = {}
for t in state.fields:
    traj[t] = np.empty((mesh.shape, num_sample + 2))
    traj[t][:, 0] = state.fields[t].data
for j in range(num_sample):
    status = euler.advance(flux, omega, state, t_sample)
    if not status:
        raise RuntimeError("Error advancing integrator")
    for t in state.fields:
        traj[t][:, j + 1] = state.fields[t].data

# repeat calculation with picard iteration
picard = flyft.solver.PicardIteration(1.0e-3, 1000, 1.0e-12)
conv = picard.solve(omega, state)
if not conv:
    picard.mix_parameter = 1.0e-2
    picard.max_iterations = 10000
    conv = picard.solve(omega, state)
for t in state.fields:
    traj[t][:, -1] = state.fields[t].data

for t in state.fields:
    np.savetxt("density.{}.dat".format(t), np.column_stack((x, traj[t])))
