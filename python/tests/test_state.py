import numpy as np
import pytest

import flyft

@pytest.fixture
def state():
    m = flyft.Mesh(10.0,20)
    return flyft.State(m,('A','B'))

def test_init(state):
    assert isinstance(state.mesh, flyft.Mesh)
    assert state.mesh.L == pytest.approx(10.0)
    assert state.mesh.shape == 20

def test_fields(state):
    assert isinstance(state.fields['A'],flyft.Field)
    assert isinstance(state.fields['B'],flyft.Field)

def test_diameters(state):
    # default values
    assert state.diameters['A'] == pytest.approx(0)
    assert state.diameters['B'] == pytest.approx(0)

    # set by key
    state.diameters['A'] = 1.0
    assert state.diameters['A'] == pytest.approx(1)

    # set by update
    state.diameters.update(B=2.0)
    assert state.diameters['B'] == pytest.approx(2)

    # set as new dict
    state.diameters = {'A': 1.5, 'B': 2.5}
    assert state.diameters['A'] == pytest.approx(1.5)
    assert state.diameters['B'] == pytest.approx(2.5)

    # check mirror class values
    assert state._self.diameters['A'] == pytest.approx(1.5)
    assert state._self.diameters['B'] == pytest.approx(2.5)
