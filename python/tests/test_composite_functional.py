import numpy as np
import pytest

import flyft
from .test_ideal_gas import ig

@pytest.fixture
def comp():
    return flyft.functional.Composite()

def test_init(ig):
    comp = flyft.functional.Composite(ig)
    assert len(comp.functionals) == 1
    assert ig in comp.functionals

def test_init_tuple(ig):
    ig2 = flyft.functional.IdealGas()
    comp = flyft.functional.Composite([ig,ig2])
    assert len(comp.functionals) == 2
    assert ig in comp.functionals
    assert ig2 in comp.functionals

def test_append_extend_remove(comp,ig):
    assert len(comp.functionals) == 0
    comp.append(ig)

    assert len(comp.functionals) == 1
    assert len(comp._self.functionals) == 1
    assert ig in comp.functionals
    assert ig._self in comp._self.functionals

    ig2 = flyft.functional.IdealGas()
    comp.append(ig2)
    assert len(comp.functionals) == 2
    assert len(comp._self.functionals) == 2

    comp.append(ig)
    assert len(comp.functionals) == 2
    assert len(comp._self.functionals) == 2

    comp.remove(ig2)
    assert len(comp.functionals) == 1
    assert len(comp._self.functionals) == 1

    comp.remove(ig)
    assert len(comp.functionals) == 0
    assert len(comp._self.functionals) == 0

    comp.extend([ig,ig2])
    assert len(comp.functionals) == 2
    assert len(comp._self.functionals) == 2

def test_compute(comp,ig):
    m = flyft.Mesh(10.0,20)
    state = flyft.State(m,'A')
    state.fields['A'][:] = 1.0

    comp = flyft.functional.Composite(ig)
    ig.volumes['A'] = 1.0

    comp.compute(state)
    assert comp.value == pytest.approx(-10.0)
    assert np.allclose(comp.derivatives['A'].data,0.0)

    ig2 = flyft.functional.IdealGas()
    ig2.volumes['A'] = 2.0
    comp.append(ig2)

    comp.compute(state)
    assert comp.value == pytest.approx(-13.068528194400546)
    assert np.allclose(comp.derivatives['A'].data,0.6931471805599453)
