import numpy as np
import pytest

import flyft

@pytest.fixture
def ig():
    return flyft.functional.IdealGas()

def test_volumes(ig):
    assert len(ig.volumes) == 0

    # set by key
    ig.volumes['A'] = 1.0
    assert ig.volumes['A'] == pytest.approx(1.0)
    assert ig._self.volumes['A'] == pytest.approx(1.0)

    # set by update
    ig.volumes.update(B=2.0)
    assert ig.volumes['A'] == pytest.approx(1.0)
    assert ig.volumes['B'] == pytest.approx(2.0)

    # set as new dict
    ig.volumes = {'A': 1.5, 'B': 2.5}
    assert ig.volumes['A'] == pytest.approx(1.5)
    assert ig.volumes['B'] == pytest.approx(2.5)

    # check mirror class values
    assert ig._self.volumes['A'] == pytest.approx(1.5)
    assert ig._self.volumes['B'] == pytest.approx(2.5)

def test_compute(ig):
    m = flyft.Mesh(10.0,20)
    state = flyft.State(m,('A','B'))

    state.fields['A'][:] = 1.0
    state.fields['B'][:] = 0.0
    ig.volumes['A'] = 1.0
    ig.volumes['B'] = 2.0

    # compute with only one component present
    ig.compute(state)
    assert ig.value == pytest.approx(-10.0)
    assert np.allclose(ig.derivatives['A'].data,0.0)
    assert np.all(ig.derivatives['B'].data == -np.inf)

    # compute with both present
    state.fields['B'][:] = 0.5
    ig.compute(state)
    assert ig.value == pytest.approx(-15.0)
    assert np.allclose(ig.derivatives['A'].data,0.0)
    assert np.allclose(ig.derivatives['B'].data,0.0)
