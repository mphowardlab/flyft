import numpy as np
import flyft

L = 50.0
dx = 0.2
diameters = {'A': 2.0, 'B': 4.0}
etas = {'A': 0.1*(77./L), 'B': 0.3*(77./L)}

# initialize mesh
padding = 0.5*max(diameters.values())
Lpad = L+2*padding
mesh = flyft.Mesh(Lpad,int(Lpad/dx))
state = flyft.State(mesh,tuple(diameters.keys()))

# setup functionals
ig = flyft.functional.IdealGas()
for t in state.fields:
    ig.volumes[t] = 1.0

fmt = flyft.functional.RosenfeldFMT()
fmt.diameters = diameters

Vext = flyft.external.Composite((flyft.external.ExponentialWall(padding,1.0),
                                 flyft.external.ExponentialWall(mesh.L-padding,-1.0)))
for p in Vext.potentials:
    for t in state.fields:
        p.epsilons[t] = 7400.
        p.kappas[t] = 2.
        p.shifts[t] = p.normal*0.5*diameters[t]

omega = flyft.functional.GrandPotential(ig,fmt,Vext)

# setup initial density based on target bulk packing fraction
x = state.mesh.coordinates
mus = {}
for t in state.fields:
    d = diameters[t]
    inside = np.logical_and(x >= Vext.potentials[0].origin,
                            x <= Vext.potentials[1].origin)
    rho = (6*etas[t])/(np.pi*d**3)
    state.fields[t][inside] = rho
    state.fields[t][~inside] = 0.

    # make the number of particles constant
    omega.constrain(t, L*rho, omega.Constraint.N)

# iterate to convergence
picard = flyft.solver.PicardIteration(1.e-4,200000,1.e-8)
picard.solve(omega,state)
