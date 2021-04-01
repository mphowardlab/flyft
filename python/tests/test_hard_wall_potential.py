import numpy as np
import pytest

import flyft

@pytest.fixture
def hw():
    return flyft.external.HardWall(2.0,True)

def test_init(hw):
    assert hw.origin == pytest.approx(2.0)
    assert hw.normal == True

def test_diameters(hw):
    assert len(hw.diameters) == 0

    # set by key
    hw.diameters['A'] = 1.0
    assert hw.diameters['A'] == pytest.approx(1.0)
    assert hw._self.diameters['A'] == pytest.approx(1.0)

    # set by update
    hw.diameters.update(B=2.0)
    assert hw.diameters['A'] == pytest.approx(1.0)
    assert hw.diameters['B'] == pytest.approx(2.0)

    # set as new dict
    hw.diameters = {'A': 1.5, 'B': 2.5}
    assert hw.diameters['A'] == pytest.approx(1.5)
    assert hw.diameters['B'] == pytest.approx(2.5)

    # check mirror class values
    assert hw._self.diameters['A'] == pytest.approx(1.5)
    assert hw._self.diameters['B'] == pytest.approx(2.5)

def test_potential(hw,mesh,state):
    hw.diameters['A'] = 1.0
    state.fields['A'][:] = 2.0

    x = state.mesh.coordinates

    # some particles overlap, so this is infinite
    hw.compute(state)
    assert hw.value == np.inf
    assert np.all(hw.derivatives['A'][x <= 2.5] == np.inf)
    assert np.allclose(hw.derivatives['A'][x > 2.5],0.0)

    # move wall outside domain, so there is no overlap & potential is zero
    hw.origin = -2.0
    hw.compute(state)
    assert hw.value == pytest.approx(0.0)
    assert np.allclose(hw.derivatives['A'].data,0.0)

    # put the wall back and switch it the other way
    hw.origin = 2.0
    hw.normal = False
    hw.compute(state)
    assert hw.value == np.inf
    assert np.allclose(hw.derivatives['A'][x <= 1.5],0.0)
    assert np.all(hw.derivatives['A'][x > 1.5] == np.inf)

    # carve out the density to get finite values
    state.fields['A'][state.mesh.coordinates >= 1.5] = 0.
    hw.compute(state)
    assert hw.value == pytest.approx(0.0)
    assert np.allclose(hw.derivatives['A'][x <= 1.5],0.0)
    assert np.all(hw.derivatives['A'][x > 1.5] == np.inf)
