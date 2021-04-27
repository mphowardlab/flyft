import numpy as np
import flyft

L = 12.0

# initialize mesh
mesh = flyft.Mesh(L,120)
state = flyft.State(mesh,'A')

# setup functionals
ig = flyft.functional.IdealGas()
ig.volumes['A'] = 1.0

Vext = flyft.external.Composite((flyft.external.HardWall(1.0,1.0),
                                 flyft.external.HardWall(L-1.0,-1.0)))
for p in Vext.potentials:
    p.diameters['A'] = 0.

omega = flyft.functional.GrandPotential(ideal=ig,external=Vext)

# setup initial density based on target bulk packing fraction
x = state.mesh.coordinates
for t in state.fields:
    inside = np.logical_and(x > Vext.potentials[0].origin,
                            x < Vext.potentials[1].origin)
    Linside = (Vext.potentials[1].origin-Vext.potentials[0].origin)
    state.fields[t][inside] = 1.0+(2.0-1.0)/Linside*(x[inside]-Vext.potentials[0].origin)
    state.fields[t][~inside] = 0.
    omega.constrain(t, 1.5*Linside, omega.Constraint.N)

flux = flyft.dynamics.BrownianDiffusiveFlux()
flux.diffusivities['A'] = 1.0

# Euler integration of trajectory
dt = 1.e-3
t_sample = 10.
num_sample = 5
sample_steps = int(t_sample/dt)

traj = np.empty((mesh.shape,num_sample+1))
traj[:,0] = state.fields['A'].data

# fourier series solution
def exact(t):
    D = 1.0
    nmax = 50
    f = np.zeros(mesh.shape)

    inside = np.logical_and(x > Vext.potentials[0].origin,
                            x < Vext.potentials[1].origin)
    Linside = (Vext.potentials[1].origin-Vext.potentials[0].origin)
    f[inside] = 1.5
    for n in range(1,nmax):
        f[inside] += 2*(np.cos(np.pi*n)-1)/(np.pi*n)**2*np.exp(-(np.pi*n)**2*D/Linside**2*t)*np.cos(np.pi*n*(x[inside]-Vext.potentials[0].origin)/Linside)
    return f
traj_exact = np.empty((mesh.shape,num_sample+1))
traj_exact[:,0] = exact(0)

for j in range(num_sample):
    print(j)
    for i in range(sample_steps):
        flux.compute(omega,state)
        jlo = flux.fluxes['A']
        jhi = np.roll(jlo,-1)
        drho = (-jhi+jlo)/mesh.step
        state.fields['A'].data += dt*drho
    traj[:,j+1] = state.fields['A'].data
    traj_exact[:,j+1] = exact(t_sample*(j+1))
