import numpy as np
import pytest

import flyft

def fex_py(eta,v):
    """Percus-Yevick free-energy density of hard spheres (compressibility route)"""
    return (eta/v)*(1.5*(1/(1-eta)**2-1)-np.log(1-eta))
def muex_py(eta):
    """Percus-Yevick excess chemical potential of hard spheres (compressibility route)"""
    return 0.5*(14*eta-13*eta**2+5*eta**3)/(1-eta)**3-np.log(1-eta)

def test_diameters(fmt):
    assert len(fmt.diameters) == 0

    # set by key
    fmt.diameters['A'] = 1.0
    assert fmt.diameters['A'] == pytest.approx(1.0)
    assert fmt._self.diameters['A'] == pytest.approx(1.0)

    # set by update
    fmt.diameters.update(B=2.0)
    assert fmt.diameters['A'] == pytest.approx(1.0)
    assert fmt.diameters['B'] == pytest.approx(2.0)

    # set as new dict
    fmt.diameters = {'A': 1.5, 'B': 2.5}
    assert fmt.diameters['A'] == pytest.approx(1.5)
    assert fmt.diameters['B'] == pytest.approx(2.5)

    # check mirror class values
    assert fmt._self.diameters['A'] == pytest.approx(1.5)
    assert fmt._self.diameters['B'] == pytest.approx(2.5)

def test_compute(fmt,binary_state):
    state = binary_state

    d = 2.0
    v = np.pi*d**3/6.
    eta = 0.1
    state.fields['A'][:] = eta/v
    state.fields['B'][:] = 0.0
    fmt.diameters['A'] = d
    fmt.diameters['B'] = 0.0

    # compute with only one component present
    fmt.compute(state)
    assert fmt.value == pytest.approx(10.*fex_py(eta,v))
    assert np.allclose(fmt.derivatives['A'].data,muex_py(eta))
    assert np.allclose(fmt.derivatives['B'].data,0.0)

    # compute with both present
    state.fields['A'][:] = 0.5*eta/v
    state.fields['B'][:] = 0.5*eta/v
    fmt.diameters['A'] = d
    fmt.diameters['B'] = d
    fmt.compute(state)
    assert fmt.value == pytest.approx(10.*fex_py(eta,v))
    assert np.allclose(fmt.derivatives['A'].data,muex_py(eta))
    assert np.allclose(fmt.derivatives['B'].data,muex_py(eta))
