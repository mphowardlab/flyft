import numpy as np
import pytest

import flyft

@pytest.fixture
def comp():
    return flyft.functional.CompositeFunctional()

def test_init(ig):
    comp = flyft.functional.CompositeFunctional(ig)
    assert len(comp.objects) == 1
    assert ig in comp.objects

def test_init_tuple(ig):
    ig2 = flyft.functional.IdealGas()
    comp = flyft.functional.CompositeFunctional([ig,ig2])
    assert len(comp.objects) == 2
    assert ig in comp.objects
    assert ig2 in comp.objects

def test_append_extend_remove(comp,ig):
    assert len(comp.objects) == 0
    comp.append(ig)

    assert len(comp.objects) == 1
    assert len(comp._self.objects) == 1
    assert ig in comp.objects
    assert ig._self in comp._self.objects

    ig2 = flyft.functional.IdealGas()
    comp.append(ig2)
    assert len(comp.objects) == 2
    assert len(comp._self.objects) == 2

    comp.append(ig)
    assert len(comp.objects) == 2
    assert len(comp._self.objects) == 2

    comp.remove(ig2)
    assert len(comp.objects) == 1
    assert len(comp._self.objects) == 1

    comp.remove(ig)
    assert len(comp.objects) == 0
    assert len(comp._self.objects) == 0

    comp.extend([ig,ig2])
    assert len(comp.objects) == 2
    assert len(comp._self.objects) == 2

def test_compute(comp,ig,state,volumes):
    state.fields['A'][:] = 1.0

    comp.append(ig)
    ig.volumes['A'] = 1.0

    comp.compute(state)
    assert comp.value == pytest.approx(-volumes)
    assert np.allclose(comp.derivatives['A'].data,0.0)

    ig2 = flyft.functional.IdealGas()
    ig2.volumes['A'] = 2.0
    comp.append(ig2)

    comp.compute(state)
    assert comp.value == pytest.approx((-13.068528194400546/10)*volumes)
    assert np.allclose(comp.derivatives['A'].data,0.6931471805599453)
