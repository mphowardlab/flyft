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

def test_advance(state,grand,ig,linear,bd,cn):
    ig.volumes['A'] = 1.0
    grand.ideal = ig
    bd.diffusivities['A'] = 2.0

    # first check OK with all ones (no change)
    state.fields['A'][:] = 1.0
    grand.constrain('A', 1.0*state.mesh.full.L, grand.Constraint.N)
    # run forward one step
    cn.advance(bd, grand, state, cn.timestep)
    assert state.time == pytest.approx(1.e-3)
    assert np.allclose(state.fields['A'], 1.0)
    # run backward one step
    cn.advance(bd, grand, state, -cn.timestep)
    assert state.time == pytest.approx(0.)
    assert np.allclose(state.fields['A'], 1.0)
    if isinstance(state.mesh.full,flyft.state.CartesianMesh):   
        # add a linear field flux, but this is bulk so there should still be no change
        # except near the edges where the potential seems discontinuous by finite difference
        linear.set_line('A', x=0., y=0., slope=0.25)
        grand.external = linear
        bd.compute(grand, state)
        cn.advance(bd, grand, state, cn.timestep)
        assert state.time == pytest.approx(1.e-3)
        assert np.allclose(state.fields['A'][2:-2], 1.0, atol=1e-4)

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

@pytest.mark.parametrize("adapt",[False,True])
def test_sine(adapt,cn,state_sine):
    state = state_sine
    x = state.mesh.local.centers
    state.fields['A'][:] = 0.5*np.sin(2*np.pi*x/state.mesh.full.L)+1.

    ig = flyft.functional.IdealGas()
    ig.volumes['A'] = 1.
    grand = flyft.functional.GrandPotential(ig)
    grand.constrain('A', 1.0*state.mesh.full.L, grand.Constraint.N)

    bd = flyft.dynamics.BrownianDiffusiveFlux()
    bd.diffusivities['A'] = 0.5

    if adapt:
        cn.adaptive = True
    else:
        cn.adaptive = False
        cn.timestep = 1.e-5

    tau = state.mesh.full.L**2/(4*np.pi**2*bd.diffusivities['A'])
    t = 1.5*tau
    cn.advance(bd, grand, state, t)    
    if isinstance(state.mesh.full,flyft.state.CartesianMesh):
        sol = 0.5*np.exp(-t/tau)*np.sin(2*np.pi*x/state.mesh.full.L)+1
        assert np.allclose(state.fields['A'],sol,atol=1.e-4)
