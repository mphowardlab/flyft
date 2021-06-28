import numpy as np
import pytest

import flyft

@pytest.fixture
def cn():
    return flyft.dynamics.CrankNicolsonIntegrator(1.e-3,1.,2,1.e-6)

def test_init(cn):
    assert cn.timestep == pytest.approx(1.e-3)
    assert cn.mix_parameter == pytest.approx(1.0)
    assert cn.max_iterations == 2
    assert cn.tolerance == pytest.approx(1.e-6)

    # change time step
    cn.timestep = 1.e-2
    assert cn.timestep == pytest.approx(1.e-2)

    # change mix param
    cn.mix_parameter = 0.05
    assert cn.mix_parameter == pytest.approx(0.05)
    assert cn._self.mix_parameter == pytest.approx(0.05)

    # change max iterations
    cn.max_iterations = 20
    assert cn.max_iterations == 20
    assert cn._self.max_iterations == 20

    # change tolerance
    cn.tolerance = 1.e-7
    assert cn.tolerance == pytest.approx(1.e-7)
    assert cn._self.tolerance == pytest.approx(1.e-7)

def test_advance(mesh,state,grand,ig,linear,bd,cn):
    ig.volumes['A'] = 1.0
    grand.ideal = ig
    bd.diffusivities['A'] = 2.0

    # first check OK with all ones (no change)
    state.fields['A'][:] = 1.0
    grand.constrain('A', 1.0*mesh.L, grand.Constraint.N)
    # run forward one step
    cn.advance(bd, grand, state, cn.timestep)
    assert state.time == pytest.approx(1.e-3)
    assert np.allclose(state.fields['A'], 1.0)
    # run backward one step
    cn.advance(bd, grand, state, -cn.timestep)
    assert state.time == pytest.approx(0.)
    assert np.allclose(state.fields['A'], 1.0)

    # add a linear field flux, but this is bulk so there should still be no change
    # except near the edges where the potential seems discontinuous by finite difference
    linear.set_line('A', x=0., y=0., slope=0.25)
    grand.external = linear
    cn.advance(bd, grand, state, cn.timestep)
    assert state.time == pytest.approx(1.e-3)
    assert np.allclose(state.fields['A'][1:-1], 1.0, atol=1e-4)

    # run forwards multiple steps
    state.time = 0.
    state.fields['A'][:] = 1.0
    grand.external = None
    cn.advance(bd, grand, state, 0.1)
    assert state.time == pytest.approx(0.1)
    cn.advance(bd, grand, state, -0.1)
    assert state.time == pytest.approx(0.)

    # run forwards for a partial step
    cn.advance(bd, grand, state, 1.5e-4)
    assert state.time == pytest.approx(1.5e-4)
    cn.advance(bd, grand, state, -1.5e-4)
    assert state.time == pytest.approx(0.)
