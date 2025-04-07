import flyft
import numpy as np
import pytest


@pytest.fixture
def lj():
    return flyft.external.LennardJones93Wall(2.0, 1.0)


def test_init(lj):
    assert lj.origin == pytest.approx(2.0)
    assert lj.normal == 1.0


def test_epsilons(lj):
    assert len(lj.epsilons) == 0

    # set by key
    lj.epsilons["A"] = 1.0
    assert lj.epsilons["A"] == pytest.approx(1.0)
    assert lj._self.epsilons["A"] == pytest.approx(1.0)

    # set by update
    lj.epsilons.update(B=2.0)
    assert lj.epsilons["A"] == pytest.approx(1.0)
    assert lj.epsilons["B"] == pytest.approx(2.0)

    # set as new dict
    lj.epsilons = {"A": 1.5, "B": 2.5}
    assert lj.epsilons["A"] == pytest.approx(1.5)
    assert lj.epsilons["B"] == pytest.approx(2.5)

    # check mirror class values
    assert lj._self.epsilons["A"] == pytest.approx(1.5)
    assert lj._self.epsilons["B"] == pytest.approx(2.5)


def test_sigmas(lj):
    assert len(lj.sigmas) == 0

    # set by key
    lj.sigmas["A"] = 1.0
    assert lj.sigmas["A"] == pytest.approx(1.0)
    assert lj._self.sigmas["A"] == pytest.approx(1.0)

    # set by update
    lj.sigmas.update(B=2.0)
    assert lj.sigmas["A"] == pytest.approx(1.0)
    assert lj.sigmas["B"] == pytest.approx(2.0)

    # set as new dict
    lj.sigmas = {"A": 1.5, "B": 2.5}
    assert lj.sigmas["A"] == pytest.approx(1.5)
    assert lj.sigmas["B"] == pytest.approx(2.5)

    # check mirror class values
    assert lj._self.sigmas["A"] == pytest.approx(1.5)
    assert lj._self.sigmas["B"] == pytest.approx(2.5)


def test_cutoffs(lj):
    assert len(lj.cutoffs) == 0

    # set by key
    lj.cutoffs["A"] = 1.0
    assert lj.cutoffs["A"] == pytest.approx(1.0)
    assert lj._self.cutoffs["A"] == pytest.approx(1.0)

    # set by update
    lj.cutoffs.update(B=2.0)
    assert lj.cutoffs["A"] == pytest.approx(1.0)
    assert lj.cutoffs["B"] == pytest.approx(2.0)

    # set as new dict
    lj.cutoffs = {"A": 1.5, "B": 2.5}
    assert lj.cutoffs["A"] == pytest.approx(1.5)
    assert lj.cutoffs["B"] == pytest.approx(2.5)

    # check mirror class values
    assert lj._self.cutoffs["A"] == pytest.approx(1.5)
    assert lj._self.cutoffs["B"] == pytest.approx(2.5)


def test_shifts(lj):
    assert len(lj.shifts) == 0

    # set by key
    lj.shifts["A"] = False
    assert lj.shifts["A"] is False
    assert lj._self.shifts["A"] is False

    # set by update
    lj.shifts.update(B=True)
    assert lj.shifts["A"] is False
    assert lj.shifts["B"] is True

    # set as new dict
    lj.shifts = {"A": True, "B": False}
    assert lj.shifts["A"] is True
    assert lj.shifts["B"] is False

    # check mirror class values
    assert lj._self.shifts["A"] is True
    assert lj._self.shifts["B"] is False


def test_origin(lj, state):
    param = flyft.parameter.LinearParameter(2.0, 0.0, 0.1)
    lj.origin = param
    assert lj.origin is param
    assert lj._self.origin is param._self

    # check origin evaluates properly at both levels
    assert lj.origin(state) == pytest.approx(2.0)
    assert lj._self.origin(state._self) == pytest.approx(2.0)

    # change time and check again
    state.time = 10.0
    assert lj.origin(state) == pytest.approx(3.0)

    # convert the origin back to a float
    lj.origin = 2.0
    assert isinstance(lj.origin, float)
    assert lj.origin == pytest.approx(2.0)

    # construct another wall using a parameter
    lj2 = flyft.external.LennardJones93Wall(param, 1.0)
    assert lj2.origin is param
    assert lj2._self.origin is param._self


def test_potential(lj, state):
    lj.epsilons["A"] = 2.0
    lj.sigmas["A"] = 1.5
    lj.cutoffs["A"] = np.inf
    lj.shifts["A"] = False
    state.fields["A"][:] = 2.0

    x = state.mesh.local.centers

    # initial state
    lj.compute(state)
    Vref = 2.0 * (
        (2.0 / 15.0) * (1.5 / np.abs(x - 2.0)) ** 9 - (1.5 / np.abs(x - 2.0)) ** 3
    )
    assert np.all(lj.derivatives["A"][x <= 2.0] == np.inf)
    assert np.allclose(lj.derivatives["A"][x > 2.0], Vref[x > 2.0])

    # flip the normal to change which part feels the potential
    lj.normal = -1.0
    lj.compute(state)
    assert np.allclose(lj.derivatives["A"][x < 2.0], Vref[x < 2.0])
    assert np.all(lj.derivatives["A"][x >= 2.0] == np.inf)

    # add cutoff
    lj.normal = 1.0
    lj.cutoffs["A"] = 3.0
    lj.compute(state)
    flags = np.logical_and(x > 2.0, x < 5.0)
    assert np.all(lj.derivatives["A"][x <= 2.0] == np.inf)
    assert np.allclose(lj.derivatives["A"][flags], Vref[flags])
    assert np.allclose(lj.derivatives["A"][x > 5.0], 0.0)

    # add shifting
    lj.shifts["A"] = True
    lj.compute(state)
    Vref -= 2.0 * ((2.0 / 15.0) * (1.5 / 3.0) ** 9 - (1.5 / 3.0) ** 3)
    assert np.all(lj.derivatives["A"][x <= 2.0] == np.inf)
    assert np.allclose(lj.derivatives["A"][flags], Vref[flags])
    assert np.allclose(lj.derivatives["A"][x > 5.0], 0.0)
