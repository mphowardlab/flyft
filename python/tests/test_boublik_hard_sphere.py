import numpy as np
import pytest

import flyft


def fex_cs(eta,d):
    """Carnahan-Starling free-energy density of hard spheres"""
    rho = 6*eta/(np.pi*d**3)
    return rho*(4*eta-3*eta**2)/(1-eta)**2
def muex_cs(eta):
    """Carnahan-Starling excess chemical potential of hard spheres"""
    return (8*eta-9*eta**2+3*eta**3)/(1-eta)**3

@pytest.fixture
def bmcsl():
    return flyft.functional.BoublikHardSphere()

def test_diameters(bmcsl):
    assert len(bmcsl.diameters) == 0

    # set by key
    bmcsl.diameters['A'] = 1.0
    assert bmcsl.diameters['A'] == pytest.approx(1.0)
    assert bmcsl._self.diameters['A'] == pytest.approx(1.0)

    # set by update
    bmcsl.diameters.update(B=2.0)
    assert bmcsl.diameters['A'] == pytest.approx(1.0)
    assert bmcsl.diameters['B'] == pytest.approx(2.0)

    # set as new dict
    bmcsl.diameters = {'A': 1.5, 'B': 2.5}
    assert bmcsl.diameters['A'] == pytest.approx(1.5)
    assert bmcsl.diameters['B'] == pytest.approx(2.5)

    # check mirror class values
    assert bmcsl._self.diameters['A'] == pytest.approx(1.5)
    assert bmcsl._self.diameters['B'] == pytest.approx(2.5)

def test_compute(bmcsl,binary_state):
    state = binary_state

    d = 2.0
    v = np.pi*d**3/6.
    eta = 0.1
    volume = state.mesh.full.volume()
    state.fields['A'][:] = eta/v
    state.fields['B'][:] = 0.0
    bmcsl.diameters['A'] = d
    bmcsl.diameters['B'] = 0.0

    # compute with only one component present, other acts like ideal gas
    bmcsl.compute(state)
    assert bmcsl.value == pytest.approx(volume*fex_cs(eta,d))
    assert np.allclose(bmcsl.derivatives['A'].data,muex_cs(eta))
    assert np.allclose(bmcsl.derivatives['B'].data,-np.log(1.-eta))

    # compute with both present, same diameter
    state.fields['A'][:] = 0.5*eta/v
    state.fields['B'][:] = 0.5*eta/v
    bmcsl.diameters['A'] = d
    bmcsl.diameters['B'] = d
    bmcsl.compute(state)
    assert bmcsl.value == pytest.approx(volume*fex_cs(eta,d))
    assert np.allclose(bmcsl.derivatives['A'].data,muex_cs(eta))
    assert np.allclose(bmcsl.derivatives['B'].data,muex_cs(eta))

    # use pysaft values as a check for true binary fluid
    state.fields['A'][:] = 0.16
    state.fields['B'][:] = 0.02
    bmcsl.diameters['A'] = 1.0
    bmcsl.diameters['B'] = 2.0
    bmcsl.compute(state)
    assert bmcsl.value == pytest.approx(volume*0.13146765540861702)
    assert np.allclose(bmcsl.derivatives['A'].data,1.2912644301468292)
    assert np.allclose(bmcsl.derivatives['B'].data,4.4254142781874695)

def test_compute_one_type(bmcsl,state):
    d = 2.0
    v = np.pi*d**3/6.
    eta = 0.1
    volume = state.mesh.full.volume()
    state.fields['A'][:] = eta/v
    bmcsl.diameters['A'] = d
    # compute with only one component present, other acts like ideal gas
    bmcsl.compute(state)
    assert bmcsl.value == pytest.approx(volume*fex_cs(eta,d))
    assert np.allclose(bmcsl.derivatives['A'].data,muex_cs(eta))
