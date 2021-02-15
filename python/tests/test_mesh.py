import numpy as np
import pytest

import flyft

@pytest.fixture
def mesh():
    return flyft.Mesh(10.0,20)

def test_properties(mesh):
    assert mesh.L == 10.0
    assert mesh.shape == 20
    assert mesh.step == pytest.approx(0.5)

def test_mirror(mesh):
    assert mesh._self.L == 10.0
    assert mesh._self.shape == 20
    assert mesh._self.step == pytest.approx(0.5)

def test_coordinates(mesh):
    assert mesh.coordinates[0] == pytest.approx(0.25)
    assert mesh.coordinates[-1] == pytest.approx(9.75)
