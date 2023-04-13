import numpy as np
import pytest

import flyft

def f_ex(B,rho):
    """Excess free-energy density of virial expansion"""
    sum_ = 0.
    for i in rho:
        for j in rho:
            sum_ += B[i,j]*rho[i]*rho[j]
    return sum_

def mu_ex(B,rho,i):
    """Excess chemical potential of virial expansion"""
    sum_ = 0.
    for j in rho:
        sum_ += 2*B[i,j]*rho[j]
    return sum_

@pytest.fixture
def virial():
    return flyft.functional.VirialExpansion()

def test_coefficients(virial):
    assert len(virial.coefficients) == 0

    # set self interaction
    virial.coefficients['A','A'] = 1.0
    assert virial.coefficients['A','A'] == pytest.approx(1.0)
    assert virial._self.coefficients['A','A'] == pytest.approx(1.0)

    # set cross interaction
    virial.coefficients['A','B'] = 2.0
    assert virial.coefficients['A','B'] == pytest.approx(2.0)
    assert virial._self.coefficients['A','B'] == pytest.approx(2.0)

    # check symmetry
    assert virial.coefficients['B','A'] == pytest.approx(2.0)
    assert virial._self.coefficients['B','A'] == pytest.approx(2.0)
    virial.coefficients['B','A'] = 1.5
    assert virial.coefficients['A','B'] == pytest.approx(1.5)
    assert virial._self.coefficients['A','B'] == pytest.approx(1.5)
    assert virial.coefficients['B','A'] == pytest.approx(1.5)
    assert virial._self.coefficients['B','A'] == pytest.approx(1.5)

    # set by update
    virial.coefficients.update({('B','B'): 2.0})
    assert virial.coefficients['A','A'] == pytest.approx(1.0)
    assert virial.coefficients['A','B'] == pytest.approx(1.5)
    assert virial.coefficients['B','B'] == pytest.approx(2.0)

    # set as new dict
    virial.coefficients = {('A','A'): 3.0, ('A','B'): 3.5, ('B','B'): 4.0}
    assert virial.coefficients['A','A'] == pytest.approx(3.0)
    assert virial.coefficients['A','B'] == pytest.approx(3.5)
    assert virial.coefficients['B','B'] == pytest.approx(4.0)

    # check mirror class values
    assert virial._self.coefficients['A','A'] == pytest.approx(3.0)
    assert virial._self.coefficients['A','B'] == pytest.approx(3.5)
    assert virial._self.coefficients['B','B'] == pytest.approx(4.0)

def test_compute(virial,binary_state,mesh):
    state = binary_state
    volume = mesh.volume()
    # compute with only one component present
    virial.coefficients = {('A','A'): 1, ('A','B'): 1, ('B','B'): 1}
    rho = {'A': 1.0, 'B': 0.0}
    state.fields['A'][:] = rho['A']
    state.fields['B'][:] = rho['B']
    virial.compute(state)
    assert virial.value == pytest.approx(volume*f_ex(virial.coefficients,rho))
    assert np.allclose(virial.derivatives['A'].data,mu_ex(virial.coefficients,rho,'A'))
    assert np.allclose(virial.derivatives['B'].data,mu_ex(virial.coefficients,rho,'B'))

    # compute with both present (same type)
    virial.coefficients = {('A','A'): 1, ('A','B'): 1, ('B','B'): 1}
    rho = {'A': 0.5, 'B': 0.5}
    state.fields['A'][:] = rho['A']
    state.fields['B'][:] = rho['B']
    virial.compute(state)
    assert virial.value == pytest.approx(volume*f_ex(virial.coefficients,rho))
    assert np.allclose(virial.derivatives['A'].data,mu_ex(virial.coefficients,rho,'A'))
    assert np.allclose(virial.derivatives['B'].data,mu_ex(virial.coefficients,rho,'B'))

    # compute with different virial coefficients and densities
    virial.coefficients = {('A','A'): 1.0, ('A','B'): 1.5**3, ('B','B'): 2**3}
    rho = {'A': 2.0, 'B': 0.5}
    state.fields['A'][:] = rho['A']
    state.fields['B'][:] = rho['B']
    virial.compute(state)
    assert virial.value == pytest.approx(volume*f_ex(virial.coefficients,rho))
    assert np.allclose(virial.derivatives['A'].data,mu_ex(virial.coefficients,rho,'A'))
    assert np.allclose(virial.derivatives['B'].data,mu_ex(virial.coefficients,rho,'B'))
