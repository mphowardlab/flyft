import numpy as np
import pytest

import flyft

@pytest.fixture
def comp():
    return flyft.dynamics.CompositeFlux()

def test_init(bd):
    comp = flyft.dynamics.CompositeFlux(bd)
    assert len(comp.objects) == 1
    assert bd in comp.objects

def test_init_tuple(bd):
    bd2 = flyft.dynamics.BrownianDiffusiveFlux()
    comp = flyft.dynamics.CompositeFlux([bd,bd2])
    assert len(comp.objects) == 2
    assert bd in comp.objects
    assert bd2 in comp.objects

def test_append_extend_remove(comp,bd):
    assert len(comp.objects) == 0
    comp.append(bd)

    assert len(comp.objects) == 1
    assert len(comp._self.objects) == 1
    assert bd in comp.objects
    assert bd._self in comp._self.objects

    bd2 = flyft.dynamics.BrownianDiffusiveFlux()
    comp.append(bd2)
    assert len(comp.objects) == 2
    assert len(comp._self.objects) == 2

    comp.append(bd)
    assert len(comp.objects) == 2
    assert len(comp._self.objects) == 2

    comp.remove(bd2)
    assert len(comp.objects) == 1
    assert len(comp._self.objects) == 1

    comp.remove(bd)
    assert len(comp.objects) == 0
    assert len(comp._self.objects) == 0

    comp.extend([bd,bd2])
    assert len(comp.objects) == 2
    assert len(comp._self.objects) == 2

def test_compute(comp,bd,grand,ig,state):
    grand.ideal = ig
    ig.volumes['A'] = 1.0

    x = state.mesh.local.centers
    state.fields['A'][:] = 3.0/state.mesh.full.L*x
    grand.constrain('A', state.mesh.full.L*1.5, grand.Constraint.N)

    comp.append(bd)
    bd.diffusivities['A'] = 1.0

    comp.compute(grand,state)
    assert np.allclose(comp.fluxes['A'][1:], -1.0*3.0/state.mesh.full.L)

    bd2 = flyft.dynamics.BrownianDiffusiveFlux()
    bd2.diffusivities['A'] = 2.0
    comp.append(bd2)

    comp.compute(grand,state)
    assert np.allclose(comp.fluxes['A'][1:], -(1.0+2.0)*3.0/state.mesh.full.L)
