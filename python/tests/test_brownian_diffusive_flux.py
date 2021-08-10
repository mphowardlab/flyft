import numpy as np
import pytest

import flyft

def test_diffusivities(bd):
    assert len(bd.diffusivities) == 0

    # set by key
    bd.diffusivities['A'] = 1.0
    assert bd.diffusivities['A'] == pytest.approx(1.0)
    assert bd._self.diffusivities['A'] == pytest.approx(1.0)

    # set by update
    bd.diffusivities.update(B=2.0)
    assert bd.diffusivities['A'] == pytest.approx(1.0)
    assert bd.diffusivities['B'] == pytest.approx(2.0)

    # set as new dict
    bd.diffusivities = {'A': 1.5, 'B': 2.5}
    assert bd.diffusivities['A'] == pytest.approx(1.5)
    assert bd.diffusivities['B'] == pytest.approx(2.5)

    # check mirror class values
    assert bd._self.diffusivities['A'] == pytest.approx(1.5)
    assert bd._self.diffusivities['B'] == pytest.approx(2.5)

def test_ideal(grand,ig,bd):
    state = flyft.State(flyft.Mesh(10.0,500),'A')

    ig.volumes['A'] = 1.0
    grand.ideal = ig
    bd.diffusivities['A'] = 2.0

    # first check OK with all zeros
    state.fields['A'][:] = 0.
    grand.constrain('A', 0, grand.Constraint.N)
    bd.compute(grand,state)
    assert np.allclose(bd.fluxes['A'], 0.)

    # same thing with all ones
    state.fields['A'][:] = 1.
    grand.constrain('A', state.mesh.full.L*1.0, grand.Constraint.N)
    bd.compute(grand,state)
    assert np.allclose(bd.fluxes['A'], 0.)

    # make linear profile and test in the middle
    # (skip first one because there is jump over PBC)
    # j = -D (drho/dx)
    x = state.mesh.local.coordinates
    state.fields['A'][:] = 3.0/state.mesh.full.L*x
    grand.constrain('A', state.mesh.full.L*1.5, grand.Constraint.N)
    bd.compute(grand,state)
    assert np.allclose(bd.fluxes['A'][1:], -2.0*3.0/state.mesh.full.L)

    # make sinusoidal profile and test with looser tolerance due to finite diff
    state.fields['A'][:] = (np.sin(2*np.pi*x/state.mesh.full.L)+1)
    grand.constrain('A', state.mesh.full.L*1., grand.Constraint.N)
    bd.compute(grand,state)
    # flux is computed at the left edge
    j = -2.0*(2.*np.pi/state.mesh.full.L)*np.cos(2*np.pi*(x-0.5*state.mesh.full.step)/state.mesh.full.L)
    assert np.allclose(bd.fluxes['A'], j, rtol=1e-3, atol=1e-2)

def test_excess(grand,ig,fmt,bd):
    state = flyft.State(flyft.Mesh(10.0,500),'A')

    ig.volumes['A'] = 1.0
    fmt.diameters['A'] = 1.0
    grand.ideal = ig
    grand.excess = fmt
    grand.constrain('A', state.mesh.full.L*1.0, grand.Constraint.N)
    bd.diffusivities['A'] = 2.0

    # first check OK with all zeros
    state.fields['A'][:] = 0.
    grand.constrain('A', 0, grand.Constraint.N)
    bd.compute(grand,state)
    assert np.allclose(bd.fluxes['A'], 0.)

    # same thing with all constant
    state.fields['A'][:] = 1.e-2
    grand.constrain('A', state.mesh.full.L*1.e-2, grand.Constraint.N)
    bd.compute(grand,state)
    assert np.allclose(bd.fluxes['A'], 0.)

    # TODO: replace this with a local hs functional instead of fmt so the test is exact
    x = state.mesh.local.coordinates
    state.fields['A'][:] = 1.e-1*(np.sin(2*np.pi*x/state.mesh.full.L)+1)
    grand.constrain('A', state.mesh.full.L*1.e-1, grand.Constraint.N)
    bd.compute(grand,state)
    # flux is computed at the left edge
    d = fmt.diameters['A']**3
    v = np.pi*d**3/6.
    rho = 1.e-1*(np.sin(2*np.pi*(x-0.5*state.mesh.full.step)/state.mesh.full.L)+1)
    drho_dx = 1.e-1*(2.*np.pi/state.mesh.full.L)*np.cos(2*np.pi*(x-0.5*state.mesh.full.step)/state.mesh.full.L)
    eta = rho*v
    dmuex_drho = ((14-26*eta+15*eta**2+3/(1-eta)*(14*eta-13*eta**2+5*eta**3))/(2*(1-eta)**3)+1/(1-eta))*v
    jid = -2.0*drho_dx
    jex = -2.0*rho*dmuex_drho*drho_dx
    j = jid + jex
    assert np.allclose(bd.fluxes['A'], j, rtol=1e-3, atol=5e-2)

def test_external(state,grand,ig,walls,linear,bd):
    ig.volumes['A'] = 1.0
    grand.ideal = ig
    for w in walls:
        w.diameters['A'] = 0.0
    grand.external = flyft.external.CompositeExternalPotential(walls)

    bd.diffusivities['A'] = 2.0

    # first check OK with all zeros
    state.fields['A'][:] = 0.
    grand.constrain('A', 0, grand.Constraint.N)
    bd.compute(grand,state)
    assert np.allclose(bd.fluxes['A'], 0.)

    # all 1s outside the walls (should give no flux into walls still)
    x = state.mesh.local.coordinates
    inside = np.logical_and(x > walls[0].origin, x < walls[1].origin)
    state.fields['A'][inside] = 1.
    grand.constrain('A', np.sum(state.fields['A'])*state.mesh.full.step, grand.Constraint.N)
    bd.compute(grand,state)
    assert np.allclose(bd.fluxes['A'], 0.)

    # try linear gradient between walls
    slope = (2.0-1.0)/(walls[1].origin-walls[0].origin)
    state.fields['A'][inside] = 1.0+slope*(x[inside]-walls[0].origin)
    grand.constrain('A', np.sum(state.fields['A'])*state.mesh.full.step, grand.Constraint.N)
    bd.compute(grand,state)
    assert np.allclose(bd.fluxes['A'][3:-2], -2.0*slope)

    # use linear potential to check gradient calculation between walls
    state.fields['A'][inside] = 3.
    grand.constrain('A', np.sum(state.fields['A'])*state.mesh.full.step, grand.Constraint.N)
    linear.set_line('A', x=walls[0].origin, y=0., slope=0.25)
    grand.external.append(linear)
    bd.compute(grand,state)
    assert np.allclose(bd.fluxes['A'][3:-2], 3.*2.*-0.25)
