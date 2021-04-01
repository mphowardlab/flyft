import numpy as np
import pytest

import flyft

@pytest.fixture
def rf_fmt():
    return flyft.functional.RosenfeldFMT()

def fex_py(eta,v):
    """Percus-Yevick free-energy density of hard spheres (compressibility route)"""
    return (eta/v)*(1.5*(1/(1-eta)**2-1)-np.log(1-eta))
def muex_py(eta):
    """Percus-Yevick excess chemical potential of hard spheres (compressibility route)"""
    return 0.5*(14*eta-13*eta**2+5*eta**3)/(1-eta)**3-np.log(1-eta)

def test_diameters(rf_fmt):
    assert len(rf_fmt.diameters) == 0

    # set by key
    rf_fmt.diameters['A'] = 1.0
    assert rf_fmt.diameters['A'] == pytest.approx(1.0)
    assert rf_fmt.diameters['A'] == pytest.approx(1.0)

    # set by update
    rf_fmt.diameters.update(B=2.0)
    assert rf_fmt.diameters['A'] == pytest.approx(1.0)
    assert rf_fmt.diameters['B'] == pytest.approx(2.0)

    # set as new dict
    rf_fmt.diameters = {'A': 1.5, 'B': 2.5}
    assert rf_fmt.diameters['A'] == pytest.approx(1.5)
    assert rf_fmt.diameters['B'] == pytest.approx(2.5)

    # check mirror class values
    assert rf_fmt.diameters['A'] == pytest.approx(1.5)
    assert rf_fmt.diameters['B'] == pytest.approx(2.5)

def test_compute(rf_fmt):
    m = flyft.Mesh(10.0,20)
    state = flyft.State(m,('A','B'))

    d = 2.0
    v = np.pi*d**3/6.
    eta = 0.1
    state.fields['A'][:] = eta/v
    state.fields['B'][:] = 0.0
    rf_fmt.diameters['A'] = d
    rf_fmt.diameters['B'] = 0.0

    # compute with only one component present
    rf_fmt.compute(state)
    assert rf_fmt.value == pytest.approx(10.*fex_py(eta,v))
    assert np.allclose(rf_fmt.derivatives['A'].data,muex_py(eta))
    assert np.allclose(rf_fmt.derivatives['B'].data,0.0)

    # compute with both present
    state.fields['A'][:] = 0.5*eta/v
    state.fields['B'][:] = 0.5*eta/v
    rf_fmt.diameters['A'] = d
    rf_fmt.diameters['B'] = d
    rf_fmt.compute(state)
    assert rf_fmt.value == pytest.approx(10.*fex_py(eta,v))
    assert np.allclose(rf_fmt.derivatives['A'].data,muex_py(eta))
    assert np.allclose(rf_fmt.derivatives['B'].data,muex_py(eta))
