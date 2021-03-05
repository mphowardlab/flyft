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
