import numpy as np
import pytest

import flyft

@pytest.fixture
def euler():
    return flyft.dynamics.ExplicitEulerIntegrator(1.e-3)

def test_timestep(euler):
    assert euler.timestep == pytest.approx(1.e-3)

    euler.timestep = 1.e-2
    assert euler.timestep == pytest.approx(1.e-2)

    # TODO: test raises error if timestep <= 0

def test_advance(state,grand,ig,linear,bd,euler):
    ig.volumes['A'] = 1.0
    grand.ideal = ig
    bd.diffusivities['A'] = 2.0

    # first check OK with all ones (no change)
    state.fields['A'][:] = 1.0
    grand.constrain('A', 1.0*state.mesh.full.L, grand.Constraint.N)
    # run forward one step
    euler.advance(bd, grand, state, euler.timestep)
    assert state.time == pytest.approx(1.e-3)
    assert np.allclose(state.fields['A'], 1.0)
    # run backward one step
    euler.advance(bd, grand, state, -euler.timestep)
    assert state.time == pytest.approx(0.)
    assert np.allclose(state.fields['A'], 1.0)

    # add a linear field flux, but this is bulk so there should still be no change
    # except near the edges where the potential seems discontinuous by finite difference
    linear.set_line('A', x=0., y=0., slope=0.25)
    grand.external = linear
    euler.advance(bd, grand, state, euler.timestep)
    assert state.time == pytest.approx(1.e-3)

    x = state.mesh.local.centers
    flags = np.logical_and(x >= state.mesh.full.centers[1], x <= state.mesh.full.centers[-2])
    assert np.allclose(state.fields['A'][flags], 1.0)

    # check everywhere using the flux computed by bd
    state.fields['A'][:] = 1.0
    bd.compute(grand,state)
    xfull = state.mesh.full.centers
    ufull = 0.25*xfull
    jfull = -2*(ufull-np.roll(ufull,1))/state.mesh.full.step
    ratefull = (jfull-np.roll(jfull,-1))
    flags = np.logical_and(xfull >= x[0], xfull <= x[-1])
    rho = state.fields['A'].data+euler.timestep*ratefull[flags]/state.mesh.full.step
    euler.advance(bd, grand, state, euler.timestep)
    assert state.time == pytest.approx(2.e-3)
    assert np.allclose(rho, state.fields['A'])

    # run forwards multiple steps
    state.time = 0.
    state.fields['A'][:] = 1.0
    grand.external = None
    euler.advance(bd, grand, state, 0.1)
    assert state.time == pytest.approx(0.1)
    euler.advance(bd, grand, state, -0.1)
    assert state.time == pytest.approx(0.)

    # run forwards for a partial step
    euler.advance(bd, grand, state, 1.5e-4)
    assert state.time == pytest.approx(1.5e-4)
    euler.advance(bd, grand, state, -1.5e-4)
    assert state.time == pytest.approx(0.)

@pytest.mark.parametrize("adapt",[False,True])
def test_sine(adapt,euler):
    mesh = flyft.state.ParallelMesh(flyft.state.CartesianMesh(2.,100,1))
    state =  flyft.State(mesh,('A'))
    # state = flyft.State(2.,100,'A')
    x = state.mesh.local.centers
    state.fields['A'][:] = 0.5*np.sin(2*np.pi*x/state.mesh.full.L)+1.

    ig = flyft.functional.IdealGas()
    ig.volumes['A'] = 1.
    grand = flyft.functional.GrandPotential(ig)
    grand.constrain('A', 1.0*state.mesh.full.L, grand.Constraint.N)

    bd = flyft.dynamics.BrownianDiffusiveFlux()
    bd.diffusivities['A'] = 0.5

    if adapt:
        euler.adaptive = True
    else:
        euler.adaptive = False
        euler.timestep = 1.e-5

    tau = state.mesh.full.L**2/(4*np.pi**2*bd.diffusivities['A'])
    t = 1.5*tau
    euler.advance(bd, grand, state, t)

    sol = 0.5*np.exp(-t/tau)*np.sin(2*np.pi*x/state.mesh.full.L)+1
    assert np.allclose(state.fields['A'],sol,atol=1.e-4)
