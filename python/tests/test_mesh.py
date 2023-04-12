import numpy as np
import pytest

import flyft

@pytest.fixture
def mesh():
    #return flyft.state.CartesianMesh(10.0,20,1)
    return flyft.state.SphericalMesh(10.0,20)

def test_init(mesh):
    assert mesh.L == 10.0
    assert mesh.shape == 20
    assert mesh.step == pytest.approx(0.5)
    assert mesh.centers[0] == pytest.approx(0.25)
    assert mesh.centers[-1] == pytest.approx(9.75)
    assert mesh.volume() == pytest.approx(4188.790204786391)#pytest.approx(10.)
    assert mesh.volume(0) == pytest.approx(0.523598775598299)#pytest.approx(0.5)
    
def test_mirror(mesh):
    assert mesh._self.L == 10.0
    assert mesh._self.shape == 20
    assert mesh._self.step == pytest.approx(0.5)

def test_immutable(mesh):
    with pytest.raises(AttributeError):
        mesh.L = 20.0
    with pytest.raises(AttributeError):
        mesh.shape = 10
    with pytest.raises(AttributeError):
        mesh.step = 1.0
