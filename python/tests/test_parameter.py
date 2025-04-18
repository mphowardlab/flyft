import flyft
import numpy as np
import pytest


class TimeParameter(flyft.CustomParameter):
    def __call__(self, state):
        return state.time


def test_custom(state, walls):
    # check custom parameter gives valid result
    state.time = 2.0
    time = TimeParameter()
    assert time(state) == pytest.approx(2.0)

    # ensure the custom parameter can be forwarded to a class that accepts a
    # DoubleParameter
    walls[0].origin = time
    assert walls[0].origin(state) == pytest.approx(2.0)
    assert walls[0]._self.origin(state) == pytest.approx(2.0)


def test_linear(state):
    state.time = 2.0

    linear = flyft.parameter.LinearParameter(3.0, 1.0, 4.5)
    assert linear(state) == pytest.approx(3.0 + 4.5 * (2.0 - 1.0))


def test_squareroot(state):
    state.time = 2.0
    squareroot = flyft.parameter.SquareRootParameter(3.0, 1.0, 4.5)
    assert squareroot(state) == pytest.approx(np.sqrt(3.0 * 3.0 + 4.5 * (2.0 - 1.0)))


def test_squareroot_exception(state):
    state.time = -3.0
    squareroot = flyft.parameter.SquareRootParameter(1.0, 1.0, 1.0)
    with pytest.raises(ValueError):
        squareroot(state)

    state.time = 2.0
    squareroot.rate = -3.0
    with pytest.raises(ValueError):
        squareroot(state)
