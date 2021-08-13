import numpy as np
import pytest

import flyft

@pytest.fixture
def euler():
    return flyft.dynamics.ImplicitEulerIntegrator(1.e-3,1.,2,1.e-6)

def test_init(euler):
    assert euler.timestep == pytest.approx(1.e-3)
    assert euler.mix_parameter == pytest.approx(1.0)
    assert euler.max_iterations == 2
    assert euler.tolerance == pytest.approx(1.e-6)

    # change time step
    euler.timestep = 1.e-2
    assert euler.timestep == pytest.approx(1.e-2)

    # change mix param
    euler.mix_parameter = 0.05
    assert euler.mix_parameter == pytest.approx(0.05)
    assert euler._self.mix_parameter == pytest.approx(0.05)

    # change max iterations
    euler.max_iterations = 20
    assert euler.max_iterations == 20
    assert euler._self.max_iterations == 20

    # change tolerance
    euler.tolerance = 1.e-7
    assert euler.tolerance == pytest.approx(1.e-7)
    assert euler._self.tolerance == pytest.approx(1.e-7)

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
    assert np.allclose(state.fields['A'][1:-1], 1.0, atol=1e-3)

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
    state = flyft.State(2.,100,'A')
    x = state.mesh.local.coordinates
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
