import numpy as np
import pytest

import flyft
from .test_ideal_gas import f_ig,mu_ig
from .test_rosenfeld_fmt import fex_py,muex_py

def test_ideal(grand):
    assert grand.ideal is None

    # set ideal gas functional
    ig = flyft.functional.IdealGas()
    ig.volumes['A'] = 2.0
    grand.ideal = ig
    assert grand.ideal is ig
    assert grand.ideal.volumes['A'] == pytest.approx(2.0)

    # change property of functional
    ig.volumes['A'] = 1.0
    assert grand.ideal.volumes['A'] == pytest.approx(1.0)

def test_excess(grand):
    assert grand.excess is None

    # set excess functional
    fmt = flyft.functional.RosenfeldFMT()
    grand.excess = fmt
    assert grand.excess is fmt

    # change property of functional
    fmt.diameters['A'] = 1.0
    assert grand.excess.diameters['A'] == pytest.approx(1.0)

def test_external(grand):
    assert grand.external is None

    hw = flyft.external.HardWall(0.0, 1.0)
    grand.external = hw
    assert grand.external is hw

    hw.diameters['A'] = 1.0
    assert grand.external.diameters['A'] == pytest.approx(1.0)

def test_constraints(grand):
    assert len(grand.constraints) == 0
    assert len(grand.constraint_types) == 0

    # set by key
    grand.constrain('A', 1.0, grand.Constraint.N)
    assert grand.constraints['A'] == pytest.approx(1.0)
    assert grand._self.constraints['A'] == pytest.approx(1.0)
    assert grand.constraint_types['A'] == grand.Constraint.N
    assert grand._self.constraint_types['A'] == grand._self.Constraint.N

    # change value
    grand.constraints['A'] = 2.0
    assert grand.constraints['A'] == pytest.approx(2.0)
    assert grand._self.constraints['A'] == pytest.approx(2.0)

    # change type
    grand.constraint_types['A'] = grand.Constraint.mu
    assert grand.constraint_types['A'] == grand.Constraint.mu
    assert grand._self.constraint_types['A'] == grand.Constraint.mu

def test_compute(grand,state):
    d = 1.0
    v = np.pi*d**3/6.
    eta = 0.1
    rho = eta/v
    state.fields['A'][:] = rho

    # check ideal contribution
    grand.ideal = flyft.functional.IdealGas()
    grand.ideal.volumes['A'] = 1.0
    grand.constrain('A', np.sum(10.0*state.fields['A'].data), grand.Constraint.N)
    grand.compute(state)
    assert grand.value == pytest.approx(10.0*f_ig(rho,1.0))
    assert np.allclose(grand.derivatives['A'].data,mu_ig(rho,1.0))

    # add excess functional
    grand.excess = flyft.functional.RosenfeldFMT()
    grand.excess.diameters['A'] = d
    grand.compute(state)
    assert grand.value == pytest.approx(10.0*(f_ig(rho,1.0)+fex_py(eta,v)))
    assert np.allclose(grand.derivatives['A'].data,mu_ig(rho,1.0)+muex_py(eta))

    # switch to constant chemical potential
    mu_bulk = 2.0
    grand.constrain('A', mu_bulk, grand.Constraint.mu)

    # result should be the same, but now there is a chemical potential term
    grand.compute(state)
    assert grand.value == pytest.approx(10.0*(f_ig(rho,1.0)+fex_py(eta,v)-mu_bulk*rho))
    assert np.allclose(grand.derivatives['A'].data,mu_ig(rho,1.0)+muex_py(eta)-mu_bulk)
