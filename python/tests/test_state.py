import numpy as np
import pytest

import flyft

def test_init(state):
    assert isinstance(state.mesh, flyft.state.ParallelMesh)
    assert state.mesh.full.L == pytest.approx(10.0)
    assert state.mesh.full.shape == 100
    assert state.time == pytest.approx(0.0)

def test_fields(binary_state):
    assert isinstance(binary_state.fields['A'],flyft.Field)
    assert isinstance(binary_state.fields['B'],flyft.Field)

def test_time(state):
    assert state.time == pytest.approx(0.0)
    assert state._self.time == pytest.approx(0.0)

    state.time = 1.0
    assert state.time == pytest.approx(1.0)
    assert state._self.time == pytest.approx(1.0)

    state.time += 0.5
    assert state.time == pytest.approx(1.5)
    assert state._self.time == pytest.approx(1.5)

def test_gather_field(state):
    state.fields['A'][:] = 1.0
    assert state.fields['A'].shape == state.mesh.local.shape

    a = state.gather_field('A')
    if state.communicator.rank == state.communicator.root:
        assert a.shape == state.mesh.full.shape
    else:
        assert a is None
