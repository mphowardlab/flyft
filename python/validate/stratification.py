import numpy as np

import flyft

L = 38.5
dx = 0.05
diameters = {"A": 1.0, "B": 4.0}
etas = {"A": 0.1, "B": 0.3}
Ds = {"A": 1.0, "B": 0.25}

# initialize mesh
padding = 0.5 * max(diameters.values())
vapor_gap = max(diameters.values())
Lpad = L + 2 * padding + vapor_gap
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

interface = flyft.external.HarmonicWall(mesh.L - padding - vapor_gap, -1.0)
for t in state.fields:
    interface.spring_constants[t] = 100.0
    interface.shifts[t] = -0.5 * diameters[t]
Vext.append(interface)

omega = flyft.functional.GrandPotential(ig, fmt, Vext)

# setup initial density inside the walls
x = state.mesh.coordinates
for t in state.fields:
    d = diameters[t]

    inside = np.logical_and(
        x >= Vext[0].origin + 0.5 * d, x <= Vext[1].origin - 0.5 * d
    )
    state.fields[t][inside] = (6 * etas[t]) / (np.pi * d**3)
    state.fields[t][~inside] = 0.0

    N = np.sum(state.fields[t]) * mesh.step
    omega.constrain(t, N, omega.Constraint.N)

# picard iteration to initial condition
try:
    # try to load all densities
    density = {}
    for t in state.fields:
        density[t] = np.loadtxt("init.{}.dat".format(t))[:, 1]

    # only actually apply if they are all available
    for t in state.fields:
        state.fields[t][:] = density[t]
    print("Loaded")
except OSError:
    picard = flyft.solver.PicardIteration(1.0e-4, 200000, 1.0e-8)
    conv = picard.solve(omega, state)
    if not conv:
        raise RuntimeError("Initial condition not converged")
    else:
        print("Converged")
        for t in state.fields:
            np.savetxt("init.{}.dat".format(t), np.column_stack((x, state.fields[t])))

# Explicit Euler time integration
v = 1.0
euler = flyft.dynamics.ExplicitEulerIntegrator(timestep=1.0e-4)
flux = flyft.dynamics.BrownianDiffusiveFlux()
flux.diffusivities = Ds
interface.origin = flyft.LinearParameter(interface.origin, 0, -v)

# perform integration up to t_final
t_sample = 5.0
t_final = 3 * t_sample
num_sample = int(t_final / t_sample)
traj = {}
for t in state.fields:
    traj[t] = np.empty((mesh.shape, num_sample + 1))
    traj[t][:, 0] = state.fields[t].data
for j in range(num_sample):
    print(j, t_sample)
    status = euler.advance(flux, omega, state, t_sample)
    if not status:
        raise RuntimeError("Error advancing integrator")
    for t in state.fields:
        traj[t][:, j + 1] = state.fields[t].data

for t in state.fields:
    np.savetxt("density.{}.dat".format(t), np.column_stack((x, traj[t])))
