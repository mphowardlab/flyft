import numpy as np
import pytest

import flyft

@pytest.fixture
def ew():
    return flyft.external.ExponentialWall(2.0,1.0)

def test_init(ew):
    assert ew.origin == pytest.approx(2.0)
    assert ew.normal == 1.0

    ew2 = flyft.external.ExponentialWall(2.0,2.0)
    assert ew2.origin == pytest.approx(2.0)
    assert ew2.normal == 1.0

    ew3 = flyft.external.ExponentialWall(4.0,-3.0)
    assert ew3.origin == pytest.approx(4.0)
    assert ew3.normal == -1.0

def test_epsilons(ew):
    assert len(ew.epsilons) == 0

    # set by key
    ew.epsilons['A'] = 1.0
    assert ew.epsilons['A'] == pytest.approx(1.0)
    assert ew._self.epsilons['A'] == pytest.approx(1.0)

    # set by update
    ew.epsilons.update(B=2.0)
    assert ew.epsilons['A'] == pytest.approx(1.0)
    assert ew.epsilons['B'] == pytest.approx(2.0)

    # set as new dict
    ew.epsilons = {'A': 1.5, 'B': 2.5}
    assert ew.epsilons['A'] == pytest.approx(1.5)
    assert ew.epsilons['B'] == pytest.approx(2.5)

    # check mirror class values
    assert ew._self.epsilons['A'] == pytest.approx(1.5)
    assert ew._self.epsilons['B'] == pytest.approx(2.5)

def test_kappas(ew):
    assert len(ew.kappas) == 0

    # set by key
    ew.kappas['A'] = 1.0
    assert ew.kappas['A'] == pytest.approx(1.0)
    assert ew._self.kappas['A'] == pytest.approx(1.0)

    # set by update
    ew.kappas.update(B=2.0)
    assert ew.kappas['A'] == pytest.approx(1.0)
    assert ew.kappas['B'] == pytest.approx(2.0)

    # set as new dict
    ew.kappas = {'A': 1.5, 'B': 2.5}
    assert ew.kappas['A'] == pytest.approx(1.5)
    assert ew.kappas['B'] == pytest.approx(2.5)

    # check mirror class values
    assert ew._self.kappas['A'] == pytest.approx(1.5)
    assert ew._self.kappas['B'] == pytest.approx(2.5)

def test_shifts(ew):
    assert len(ew.shifts) == 0

    # set by key
    ew.shifts['A'] = 1.0
    assert ew.shifts['A'] == pytest.approx(1.0)
    assert ew._self.shifts['A'] == pytest.approx(1.0)

    # set by update
    ew.shifts.update(B=2.0)
    assert ew.shifts['A'] == pytest.approx(1.0)
    assert ew.shifts['B'] == pytest.approx(2.0)

    # set as new dict
    ew.shifts = {'A': 1.5, 'B': 2.5}
    assert ew.shifts['A'] == pytest.approx(1.5)
    assert ew.shifts['B'] == pytest.approx(2.5)

    # check mirror class values
    assert ew._self.shifts['A'] == pytest.approx(1.5)
    assert ew._self.shifts['B'] == pytest.approx(2.5)

def test_origin(ew,state):
    param = flyft.parameter.LinearParameter(2.0,0.0,0.1)
    ew.origin = param
    assert ew.origin is param
    assert ew._self.origin is param._self

    # check origin evaluates properly at both levels
    assert ew.origin(state) == pytest.approx(2.0)
    assert ew._self.origin(state._self) == pytest.approx(2.0)

    # change time and check again
    state.time = 10.0
    assert ew.origin(state) == pytest.approx(3.0)

    # convert the origin back to a float
    ew.origin = 2.0
    assert isinstance(ew.origin, float)
    assert ew.origin == pytest.approx(2.0)

    # construct another wall using a parameter
    ew2 = flyft.external.ExponentialWall(param, 1.0)
    assert ew2.origin is param
    assert ew2._self.origin is param._self

def test_potential(ew,mesh,state):
    ew.epsilons['A'] = 4.0
    ew.kappas['A'] = 1.5
    ew.shifts['A'] = 0.0
    state.fields['A'][:] = 2.0

    x = state.mesh.coordinates

    # initial state
    ew.compute(state)
    Vref = 4.0*np.exp(-1.5*(x-2.0))
    assert np.allclose(ew.derivatives['A'],Vref)

    # flip the normal to change which part feels the potential
    ew.normal = -1.0
    ew.compute(state)
    Vref = 4.0*np.exp(-1.5*(2.0-x))
    assert np.allclose(ew.derivatives['A'],Vref)

    # shift the original potential
    ew.normal = 1.0
    ew.shifts['A'] = 0.5
    ew.compute(state)
    Vref = 4.0*np.exp(-1.5*(x-2.5))
    assert np.allclose(ew.derivatives['A'],Vref)
