import numpy as np
import pytest


def test_xs(linear):
    assert len(linear.xs) == 0

    # set by key
    linear.xs["A"] = 1.0
    assert linear.xs["A"] == pytest.approx(1.0)
    assert linear._self.xs["A"] == pytest.approx(1.0)

    # set by update
    linear.xs.update(B=2.0)
    assert linear.xs["A"] == pytest.approx(1.0)
    assert linear.xs["B"] == pytest.approx(2.0)

    # set as nlinear dict
    linear.xs = {"A": 1.5, "B": 2.5}
    assert linear.xs["A"] == pytest.approx(1.5)
    assert linear.xs["B"] == pytest.approx(2.5)

    # check mirror class values
    assert linear._self.xs["A"] == pytest.approx(1.5)
    assert linear._self.xs["B"] == pytest.approx(2.5)


def test_ys(linear):
    assert len(linear.ys) == 0

    # set by key
    linear.ys["A"] = 1.0
    assert linear.ys["A"] == pytest.approx(1.0)
    assert linear._self.ys["A"] == pytest.approx(1.0)

    # set by update
    linear.ys.update(B=2.0)
    assert linear.ys["A"] == pytest.approx(1.0)
    assert linear.ys["B"] == pytest.approx(2.0)

    # set as nlinear dict
    linear.ys = {"A": 1.5, "B": 2.5}
    assert linear.ys["A"] == pytest.approx(1.5)
    assert linear.ys["B"] == pytest.approx(2.5)

    # check mirror class values
    assert linear._self.ys["A"] == pytest.approx(1.5)
    assert linear._self.ys["B"] == pytest.approx(2.5)


def test_slopes(linear):
    assert len(linear.slopes) == 0

    # set by key
    linear.slopes["A"] = 1.0
    assert linear.slopes["A"] == pytest.approx(1.0)
    assert linear._self.slopes["A"] == pytest.approx(1.0)

    # set by update
    linear.slopes.update(B=2.0)
    assert linear.slopes["A"] == pytest.approx(1.0)
    assert linear.slopes["B"] == pytest.approx(2.0)

    # set as nlinear dict
    linear.slopes = {"A": 1.5, "B": 2.5}
    assert linear.slopes["A"] == pytest.approx(1.5)
    assert linear.slopes["B"] == pytest.approx(2.5)

    # check mirror class values
    assert linear._self.slopes["A"] == pytest.approx(1.5)
    assert linear._self.slopes["B"] == pytest.approx(2.5)


def test_potential(linear, state):
    linear.xs["A"] = 2.0
    linear.ys["A"] = 10.0
    linear.slopes["A"] = 0.5
    state.fields["A"][:] = 2.0

    x = state.mesh.local.centers

    # initial state
    linear.compute(state)
    Vref = 10.0 + 0.5 * (x - 2.0)
    assert np.allclose(linear.derivatives["A"], Vref)
