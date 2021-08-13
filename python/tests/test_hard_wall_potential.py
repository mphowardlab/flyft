import numpy as np
import pytest

import flyft

@pytest.fixture
def hw():
    return flyft.external.HardWall(2.0,1.0)

def test_init(hw):
    assert hw.origin == pytest.approx(2.0)
    assert hw.normal == 1.0

    hw2 = flyft.external.HardWall(2.0,2.0)
    assert hw2.origin == pytest.approx(2.0)
    assert hw2.normal == 1.0

    hw3 = flyft.external.HardWall(4.0,-3.0)
    assert hw3.origin == pytest.approx(4.0)
    assert hw3.normal == -1.0

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

def test_origin(hw,state):
    param = flyft.parameter.LinearParameter(2.0,0.0,0.1)
    hw.origin = param
    assert hw.origin is param
    assert hw._self.origin is param._self

    # check origin evaluates properly at both levels
    assert hw.origin(state) == pytest.approx(2.0)
    assert hw._self.origin(state._self) == pytest.approx(2.0)

    # change time and check again
    state.time = 10.0
    assert hw.origin(state) == pytest.approx(3.0)

    # convert the origin back to a float
    hw.origin = 2.0
    assert isinstance(hw.origin, float)
    assert hw.origin == pytest.approx(2.0)

    # construct another wall using a parameter
    hw2 = flyft.external.HardWall(param, 1.0)
    assert hw2.origin is param
    assert hw2._self.origin is param._self

def test_potential(hw,state):
    hw.diameters['A'] = 1.0
    state.fields['A'][:] = 2.0

    x = state.mesh.local.coordinates

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
    hw.normal = -1.0
    hw.compute(state)
    assert hw.value == np.inf
    assert np.allclose(hw.derivatives['A'][x <= 1.5],0.0)
    assert np.all(hw.derivatives['A'][x > 1.5] == np.inf)

    # carve out the density to get finite values
    state.fields['A'][x >= 1.5] = 0.
    hw.compute(state)
    assert hw.value == pytest.approx(0.0)
    assert np.allclose(hw.derivatives['A'][x <= 1.5],0.0)
    assert np.all(hw.derivatives['A'][x > 1.5] == np.inf)
