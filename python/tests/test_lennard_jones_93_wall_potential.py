import numpy as np
import pytest

import flyft

@pytest.fixture
def lj():
    return flyft.external.LennardJones93Wall(2.0,1.0)

def test_init(lj):
    assert lj.origin == pytest.approx(2.0)
    assert lj.normal == 1.0

    lj2 = flyft.external.LennardJones93Wall(2.0,2.0)
    assert lj2.origin == pytest.approx(2.0)
    assert lj2.normal == 1.0

    lj3 = flyft.external.LennardJones93Wall(4.0,-3.0)
    assert lj3.origin == pytest.approx(4.0)
    assert lj3.normal == -1.0

def test_epsilons(lj):
    assert len(lj.epsilons) == 0

    # set by key
    lj.epsilons['A'] = 1.0
    assert lj.epsilons['A'] == pytest.approx(1.0)
    assert lj._self.epsilons['A'] == pytest.approx(1.0)

    # set by update
    lj.epsilons.update(B=2.0)
    assert lj.epsilons['A'] == pytest.approx(1.0)
    assert lj.epsilons['B'] == pytest.approx(2.0)

    # set as new dict
    lj.epsilons = {'A': 1.5, 'B': 2.5}
    assert lj.epsilons['A'] == pytest.approx(1.5)
    assert lj.epsilons['B'] == pytest.approx(2.5)

    # check mirror class values
    assert lj._self.epsilons['A'] == pytest.approx(1.5)
    assert lj._self.epsilons['B'] == pytest.approx(2.5)

def test_sigmas(lj):
    assert len(lj.sigmas) == 0

    # set by key
    lj.sigmas['A'] = 1.0
    assert lj.sigmas['A'] == pytest.approx(1.0)
    assert lj._self.sigmas['A'] == pytest.approx(1.0)

    # set by update
    lj.sigmas.update(B=2.0)
    assert lj.sigmas['A'] == pytest.approx(1.0)
    assert lj.sigmas['B'] == pytest.approx(2.0)

    # set as new dict
    lj.sigmas = {'A': 1.5, 'B': 2.5}
    assert lj.sigmas['A'] == pytest.approx(1.5)
    assert lj.sigmas['B'] == pytest.approx(2.5)

    # check mirror class values
    assert lj._self.sigmas['A'] == pytest.approx(1.5)
    assert lj._self.sigmas['B'] == pytest.approx(2.5)

def test_cutoffs(lj):
    assert len(lj.cutoffs) == 0

    # set by key
    lj.cutoffs['A'] = 1.0
    assert lj.cutoffs['A'] == pytest.approx(1.0)
    assert lj._self.cutoffs['A'] == pytest.approx(1.0)

    # set by update
    lj.cutoffs.update(B=2.0)
    assert lj.cutoffs['A'] == pytest.approx(1.0)
    assert lj.cutoffs['B'] == pytest.approx(2.0)

    # set as new dict
    lj.cutoffs = {'A': 1.5, 'B': 2.5}
    assert lj.cutoffs['A'] == pytest.approx(1.5)
    assert lj.cutoffs['B'] == pytest.approx(2.5)

    # check mirror class values
    assert lj._self.cutoffs['A'] == pytest.approx(1.5)
    assert lj._self.cutoffs['B'] == pytest.approx(2.5)

def test_shifts(lj):
    assert len(lj.shifts) == 0

    # set by key
    lj.shifts['A'] = False
    assert lj.shifts['A'] == False
    assert lj._self.shifts['A'] == False

    # set by update
    lj.shifts.update(B=True)
    assert lj.shifts['A'] == False
    assert lj.shifts['B'] == True

    # set as new dict
    lj.shifts = {'A': True, 'B': False}
    assert lj.shifts['A'] == True
    assert lj.shifts['B'] == False

    # check mirror class values
    assert lj._self.shifts['A'] == True
    assert lj._self.shifts['B'] == False

def test_potential(lj,mesh,state):
    lj.epsilons['A'] = 2.0
    lj.sigmas['A'] = 1.5
    lj.cutoffs['A'] = np.inf
    lj.shifts['A'] = False
    state.fields['A'][:] = 2.0

    x = state.mesh.coordinates

    # initial state
    lj.compute(state)
    Vref = 2.0*((2./15.)*(1.5/np.abs(x-2.0))**9-(1.5/np.abs(x-2.0))**3)
    assert np.all(lj.derivatives['A'][x <= 2.0] == np.inf)
    assert np.allclose(lj.derivatives['A'][x > 2.0], Vref[x > 2.0])

    # flip the normal to change which part feels the potential
    lj.normal = -1.0
    lj.compute(state)
    assert np.allclose(lj.derivatives['A'][x < 2.0], Vref[x < 2.0])
    assert np.all(lj.derivatives['A'][x >= 2.0] == np.inf)

    # add cutoff
    lj.normal = 1.0
    lj.cutoffs['A'] = 3.0
    lj.compute(state)
    flags = np.logical_and(x > 2.0, x < 5.0)
    assert np.all(lj.derivatives['A'][x <= 2.0] == np.inf)
    assert np.allclose(lj.derivatives['A'][flags], Vref[flags])
    assert np.allclose(lj.derivatives['A'][x > 5.0], 0.0)

    # add shifting
    lj.shifts['A'] = True
    lj.compute(state)
    Vref -= 2.0*((2./15.)*(1.5/3.0)**9-(1.5/3.0)**3)
    assert np.all(lj.derivatives['A'][x <= 2.0] == np.inf)
    assert np.allclose(lj.derivatives['A'][flags], Vref[flags])
    assert np.allclose(lj.derivatives['A'][x > 5.0], 0.0)
