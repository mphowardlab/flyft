import numpy as np
import pytest

import flyft


def test_init(mesh):
    assert mesh.L == 10.0
    assert mesh.shape == 20
    assert mesh.step == pytest.approx(0.5)
    assert mesh.centers[0] == pytest.approx(0.25)
    assert mesh.centers[-1] == pytest.approx(9.75)

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

def test_volumes(spherical_mesh,cartesian_mesh):
    assert cartesian_mesh.volume() == pytest.approx(10.)
    assert cartesian_mesh.volume(0) == pytest.approx(0.5)
    assert spherical_mesh.volume() == pytest.approx(4188.790204786391)
    assert spherical_mesh.volume(0) == pytest.approx(0.523598775598299)
