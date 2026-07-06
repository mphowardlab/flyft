import flyft
import numpy as np

L = 10.0
dx = 0.05
diameters = {"A": 1.0}
etas = {"A": 0.1}
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

interface = flyft.external.HarmonicWall(mesh.L - padding, -1.0)
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
picard = flyft.solver.PicardIteration(1.0e-3, 1000, 1.0e-12)
conv = picard.solve(omega, state)
if not conv:
    picard.mix_parameter = 1.0e-2
    picard.max_iterations = 10000
    conv = picard.solve(omega, state)

# Explicit Euler time integration
v = 5.0
euler = flyft.dynamics.ExplicitEulerIntegrator(timestep=1.0e-4)
flux = flyft.dynamics.BrownianDiffusiveFlux()
flux.diffusivities = Ds
interface.origin = flyft.LinearParameter(interface.origin, 0, -v)

# perform integration up to t_final, where the average packing fraction is roughly 0.6
t_sample = 0.1 * (L / v)
t_final = (L / v) * (1 - sum(etas.values()) / 0.6)
num_sample = int(t_final / t_sample)
traj = {}
for t in state.fields:
    traj[t] = np.empty((mesh.shape, num_sample + 1))
    traj[t][:, 0] = state.fields[t].data
for j in range(num_sample):
    status = euler.advance(flux, omega, state, t_sample)
    if not status:
        raise RuntimeError("Error advancing integrator")
    for t in state.fields:
        traj[t][:, j + 1] = state.fields[t].data

for t in state.fields:
    np.savetxt("density.{}.dat".format(t), np.column_stack((x, traj[t])))
