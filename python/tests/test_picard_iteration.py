import numpy as np
import pytest

import flyft
from .test_ideal_gas import mu_ig
from .test_rosenfeld_fmt import muex_py

@pytest.fixture
def piccard():
    return flyft.solver.PicardIteration(0.1,100,1.e-6)

def test_init(piccard):
    assert piccard.mix_parameter == pytest.approx(0.1)
    assert piccard.max_iterations == 100
    assert piccard.tolerance == pytest.approx(1.e-6)

    # change mix param
    piccard.mix_parameter = 0.05
    assert piccard.mix_parameter == pytest.approx(0.05)
    assert piccard._self.mix_parameter == pytest.approx(0.05)

    # change max iterations
    piccard.max_iterations = 20
    assert piccard.max_iterations == 20
    assert piccard._self.max_iterations == 20

    # change tolerance
    piccard.tolerance = 1.e-7
    assert piccard.tolerance == pytest.approx(1.e-7)
    assert piccard._self.tolerance == pytest.approx(1.e-7)

def test_solve(piccard,grand,fmt,walls,state):
    rho = 0.1
    grand.ideal = flyft.functional.IdealGas()
    grand.ideal.volumes['A'] = 1.0
    grand.constrain('A', mu_ig(rho,1.0), grand.Constraint.mu)

    # solve in bulk (no walls)
    conv = piccard.solve(grand, state)
    assert conv
    assert np.allclose(state.fields['A'].data, rho, atol=1e-5)

    # add walls
    for w in walls:
        w.diameters['A'] = 0.0
    Vext = flyft.external.CompositeExternalPotential(walls)
    grand.external = Vext
    conv = piccard.solve(grand, state)
    assert conv
    x = state.mesh.local.centers
    flags = np.logical_and(x > 1.0, x <= 9.0)
    assert np.allclose(state.fields['A'][flags], rho, atol=1e-5)
    assert np.allclose(state.fields['A'][~flags], 0.0, atol=1e-5)

    # remove walls and do bulk hard spheres
    fmt.diameters['A'] = 1.0
    v = np.pi/6.
    grand.constraints['A'] = mu_ig(rho,1.0) + muex_py(rho*v)
    grand.external = None
    # first make sure we get the WRONG answer if we don't add the excess
    conv = piccard.solve(grand,state)
    assert conv
    assert not np.allclose(state.fields['A'].data, rho, atol=1e-5)
    # then add it and get the right answer
    grand.excess = fmt
    conv = piccard.solve(grand,state)
    assert conv
    assert np.allclose(state.fields['A'].data, rho, atol=1e-5)

    # last, go back to an ideal guess with N constraint
    # <N> = 2.4 would be density 0.3 in the available space (8)
    grand.excess = None
    grand.external = Vext
    grand.constrain('A', 0.3*8, grand.Constraint.N)
    conv = piccard.solve(grand,state)
    assert conv
    assert np.allclose(state.fields['A'][flags], 0.3, atol=1e-5)
    assert np.allclose(state.fields['A'][~flags], 0.0, atol=1e-5)
