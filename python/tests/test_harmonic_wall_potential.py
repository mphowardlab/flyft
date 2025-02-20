import flyft
import numpy as np
import pytest


@pytest.fixture
def hw():
    return flyft.external.HarmonicWall(2.0, 1.0)


def test_init(hw):
    assert hw.origin == pytest.approx(2.0)
    assert hw.normal == 1.0


def test_spring_constants(hw):
    assert len(hw.spring_constants) == 0

    # set by key
    hw.spring_constants["A"] = 1.0
    assert hw.spring_constants["A"] == pytest.approx(1.0)
    assert hw._self.spring_constants["A"] == pytest.approx(1.0)

    # set by update
    hw.spring_constants.update(B=2.0)
    assert hw.spring_constants["A"] == pytest.approx(1.0)
    assert hw.spring_constants["B"] == pytest.approx(2.0)

    # set as new dict
    hw.spring_constants = {"A": 1.5, "B": 2.5}
    assert hw.spring_constants["A"] == pytest.approx(1.5)
    assert hw.spring_constants["B"] == pytest.approx(2.5)

    # check mirror class values
    assert hw._self.spring_constants["A"] == pytest.approx(1.5)
    assert hw._self.spring_constants["B"] == pytest.approx(2.5)


def test_shifts(hw):
    assert len(hw.shifts) == 0

    # set by key
    hw.shifts["A"] = 1.0
    assert hw.shifts["A"] == pytest.approx(1.0)
    assert hw._self.shifts["A"] == pytest.approx(1.0)

    # set by update
    hw.shifts.update(B=2.0)
    assert hw.shifts["A"] == pytest.approx(1.0)
    assert hw.shifts["B"] == pytest.approx(2.0)

    # set as new dict
    hw.shifts = {"A": 1.5, "B": 2.5}
    assert hw.shifts["A"] == pytest.approx(1.5)
    assert hw.shifts["B"] == pytest.approx(2.5)

    # check mirror class values
    assert hw._self.shifts["A"] == pytest.approx(1.5)
    assert hw._self.shifts["B"] == pytest.approx(2.5)


def test_origin(hw, state):
    param = flyft.parameter.LinearParameter(2.0, 0.0, 0.1)
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
    hw2 = flyft.external.HarmonicWall(param, 1.0)
    assert hw2.origin is param
    assert hw2._self.origin is param._self


def test_potential(hw, state):
    hw.spring_constants["A"] = 2.0
    hw.shifts["A"] = 0.0
    state.fields["A"][:] = 2.0

    x = state.mesh.local.centers

    # initial state
    hw.compute(state)
    Vref = 0.5 * 2.0 * (x - 2.0) ** 2
    assert np.allclose(hw.derivatives["A"][x <= 2.0], Vref[x <= 2.0])
    assert np.allclose(hw.derivatives["A"][x > 2.0], 0.0)

    # flip the normal to change which part feels the potential
    hw.normal = -1.0
    hw.compute(state)
    assert np.allclose(hw.derivatives["A"][x < 2.0], 0.0)
    assert np.allclose(hw.derivatives["A"][x >= 2.0], Vref[x >= 2.0])

    # shift the original potential
    hw.normal = 1.0
    hw.shifts["A"] = 0.5
    hw.compute(state)
    Vref = 0.5 * 2.0 * (x - 2.5) ** 2
    assert np.allclose(hw.derivatives["A"][x <= 2.5], Vref[x <= 2.5])
    assert np.allclose(hw.derivatives["A"][x > 2.5], 0.0)
