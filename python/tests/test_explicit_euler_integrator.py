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

def test_advance(mesh,state,grand,ig,linear,bd,euler):
    ig.volumes['A'] = 1.0
    grand.ideal = ig
    bd.diffusivities['A'] = 2.0

    # first check OK with all ones (no change)
    state.fields['A'][:] = 1.0
    grand.constrain('A', 1.0*mesh.L, grand.Constraint.N)
    # run forward one step
    euler.advance(bd, grand, state, euler.timestep)
    assert np.allclose(state.fields['A'], 1.0)
    # run backward one step
    euler.advance(bd, grand, state, -euler.timestep)
    assert np.allclose(state.fields['A'], 1.0)

    # add a linear field flux, but this is bulk so there should still be no change
    # except near the edges where the potential seems discontinuous by finite difference
    linear.set_line('A', x=0., y=0., slope=0.25)
    grand.external = linear
    euler.advance(bd, grand, state, euler.timestep)
    assert np.allclose(state.fields['A'][1:-1], 1.0)

    # check everywhere using the flux computed by bd
    state.fields['A'][:] = 1.0
    bd.compute(grand,state)
    left = bd.fluxes['A']
    right = np.roll(bd.fluxes['A'], -1)
    rho = state.fields['A'].data+euler.timestep*(left-right)/mesh.step
    euler.advance(bd, grand, state, euler.timestep)
    assert np.allclose(rho, state.fields['A'])

    # run forwards multiple steps
    state.fields['A'][:] = 1.0
    grand.external = None
    euler.advance(bd, grand, state, 0.1)
    euler.advance(bd, grand, state, -0.1)

    # run forwards for a partial step
    euler.advance(bd, grand, state, 1.5e-4)
    euler.advance(bd, grand, state, -1.5e-4)
