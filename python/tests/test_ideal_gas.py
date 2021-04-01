import numpy as np
import pytest

import flyft

def f_ig(rho,lam):
    """Free-energy density of ideal gas"""
    if rho > 0:
        return rho*(np.log(lam*rho)-1)
    else:
        return 0.
def mu_ig(rho,lam):
    """Chemical potential of ideal gas"""
    if rho > 0:
        return np.log(lam*rho)
    else:
        return -np.inf

def test_volumes(ig):
    assert len(ig.volumes) == 0

    # set by key
    ig.volumes['A'] = 1.0
    assert ig.volumes['A'] == pytest.approx(1.0)
    assert ig._self.volumes['A'] == pytest.approx(1.0)

    # set by update
    ig.volumes.update(B=2.0)
    assert ig.volumes['A'] == pytest.approx(1.0)
    assert ig.volumes['B'] == pytest.approx(2.0)

    # set as new dict
    ig.volumes = {'A': 1.5, 'B': 2.5}
    assert ig.volumes['A'] == pytest.approx(1.5)
    assert ig.volumes['B'] == pytest.approx(2.5)

    # check mirror class values
    assert ig._self.volumes['A'] == pytest.approx(1.5)
    assert ig._self.volumes['B'] == pytest.approx(2.5)

def test_compute(ig,mesh,binary_state):
    state = binary_state

    state.fields['A'][:] = 1.0
    state.fields['B'][:] = 0.0
    ig.volumes['A'] = 1.0
    ig.volumes['B'] = 2.0

    # compute with only one component present
    ig.compute(state)
    assert ig.value == pytest.approx(10.0*f_ig(1.0,1.0))
    assert np.allclose(ig.derivatives['A'].data,mu_ig(1.0,1.0))
    assert np.all(ig.derivatives['B'].data == mu_ig(0.0,2.0))

    # compute with both present
    state.fields['B'][:] = 0.5
    ig.compute(state)
    assert ig.value == pytest.approx(10.0*(f_ig(1.0,1.0)+f_ig(0.5,2.0)))
    assert np.allclose(ig.derivatives['A'].data,mu_ig(1.0,1.0))
    assert np.allclose(ig.derivatives['B'].data,mu_ig(0.5,2.0))
