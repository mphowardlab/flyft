import numpy as np
import pytest

import flyft

@pytest.fixture
def comp():
    return flyft.external.Composite()

def test_init(walls):
    comp = flyft.external.Composite(walls[0])
    assert len(comp.potentials) == 1
    assert walls[0] in comp.potentials

def test_init_tuple(walls):
    comp = flyft.external.Composite(walls)
    assert len(comp.potentials) == 2
    assert walls[0] in comp.potentials
    assert walls[1] in comp.potentials

def test_append_extend_remove(comp,walls):
    lo,hi = walls
    assert len(comp.potentials) == 0
    comp.append(lo)

    assert len(comp.potentials) == 1
    assert len(comp._self.potentials) == 1
    assert lo in comp.potentials
    assert lo._self in comp._self.potentials

    comp.append(hi)
    assert len(comp.potentials) == 2
    assert len(comp._self.potentials) == 2

    comp.append(lo)
    assert len(comp.potentials) == 2
    assert len(comp._self.potentials) == 2

    comp.remove(hi)
    assert len(comp.potentials) == 1
    assert len(comp._self.potentials) == 1

    comp.remove(lo)
    assert len(comp.potentials) == 0
    assert len(comp._self.potentials) == 0

    comp.extend(walls)
    assert len(comp.potentials) == 2
    assert len(comp._self.potentials) == 2

def test_compute(comp,walls,mesh,state):
    x = state.mesh.coordinates
    state.fields['A'][:] = 1.0

    lo,hi = walls
    lo.diameters['A'] = 0.0
    hi.diameters['A'] = 0.0

    # assert all zeros
    comp.compute(state)
    assert np.allclose(comp.derivatives['A'].data,0.0)

    # add one wall
    comp.append(lo)
    comp.compute(state)
    assert np.all(comp.derivatives['A'][x <= 1.0] == np.inf)
    assert np.allclose(comp.derivatives['A'][x > 1.0],0.0)

    # swap lo for hi
    comp.remove(lo)
    comp.append(hi)
    comp.compute(state)
    assert np.all(comp.derivatives['A'][x > 9.0] == np.inf)
    assert np.allclose(comp.derivatives['A'][x <= 9.0],0.0)

    # add both walls
    comp.append(lo)
    comp.compute(state)
    flags = np.logical_or(x <= 1.0, x > 9.0)
    assert np.all(comp.derivatives['A'][flags] == np.inf)
    assert np.allclose(comp.derivatives['A'][~flags],0.0)
