import numpy as np
import pytest

import flyft

def test_init(state):
    assert isinstance(state.mesh, flyft.Mesh)
    assert state.mesh.L == pytest.approx(10.0)
    assert state.mesh.shape == 20

def test_fields(binary_state):
    assert isinstance(binary_state.fields['A'],flyft.Field)
    assert isinstance(binary_state.fields['B'],flyft.Field)
