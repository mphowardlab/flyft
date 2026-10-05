import flyft
import numpy as np
import pytest


def fex_cs(eta, v):
    """Carnahan--Starling free-energy density of hard spheres"""
    return (eta / v) * (4 * eta - 3 * eta**2) / (1 - eta) ** 2


def muex_cs(eta):
    """Carnahan-Starling excess chemical potential of hard spheres"""
    return (8 * eta - 9 * eta**2 + 3 * eta**3) / (1 - eta) ** 3


def test_diameters():
    fmt = flyft.functional.WhiteBearMarkII()
    assert len(fmt.diameters) == 0

    # set by key
    fmt.diameters["A"] = 1.0
    assert fmt.diameters["A"] == pytest.approx(1.0)
    assert fmt._self.diameters["A"] == pytest.approx(1.0)

    # set by update
    fmt.diameters.update(B=2.0)
    assert fmt.diameters["A"] == pytest.approx(1.0)
    assert fmt.diameters["B"] == pytest.approx(2.0)

    # set as new dict
    fmt.diameters = {"A": 1.5, "B": 2.5}
    assert fmt.diameters["A"] == pytest.approx(1.5)
    assert fmt.diameters["B"] == pytest.approx(2.5)

    # check mirror class values
    assert fmt._self.diameters["A"] == pytest.approx(1.5)
    assert fmt._self.diameters["B"] == pytest.approx(2.5)


def test_compute(binary_state):
    fmt = flyft.functional.WhiteBearMarkII()
    state = binary_state
    volume = state.mesh.full.volume()
    d = 2.0
    v = np.pi * d**3 / 6.0
    eta = 0.1
    state.fields["A"][:] = eta / v
    state.fields["B"][:] = 0.0
    fmt.diameters["A"] = d
    fmt.diameters["B"] = 0.0

    # compute with only one component present
    fmt.compute(state)
    assert fmt.value == pytest.approx(volume * fex_cs(eta, v), abs=1e-3)
    assert np.allclose(fmt.derivatives["A"].data, muex_cs(eta), atol=1e-3)
    assert np.allclose(fmt.derivatives["B"].data, 0.0, atol=1e-3)

    # compute with both present
    state.fields["A"][:] = 0.5 * eta / v
    state.fields["B"][:] = 0.5 * eta / v
    fmt.diameters["A"] = d
    fmt.diameters["B"] = d
    fmt.compute(state)
    assert fmt.value == pytest.approx(volume * fex_cs(eta, v), abs=1e-3)
    assert np.allclose(fmt.derivatives["A"].data, muex_cs(eta), atol=1e-3)
    assert np.allclose(fmt.derivatives["B"].data, muex_cs(eta), atol=1e-3)
