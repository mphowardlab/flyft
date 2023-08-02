import numpy as np
import pytest

import flyft


def test_init(mesh):
    assert mesh.L == 10.0
    assert mesh.shape == 100
    assert mesh.step == pytest.approx(0.1)
    assert mesh.lower_bound() == 0
    assert mesh.upper_bound() == 10.0
    assert mesh.centers[0] == pytest.approx(0.05)
    assert mesh.centers[-1] == pytest.approx(9.95)

def test_mirror(mesh):
    assert mesh._self.L == 10.0
    assert mesh._self.shape == 100
    assert mesh._self.step == pytest.approx(0.1)

def test_immutable(mesh):
    with pytest.raises(AttributeError):
        mesh.L = 20.0
    with pytest.raises(AttributeError):
        mesh.shape = 100
    with pytest.raises(AttributeError):
        mesh.step = 1.0

def test_volume_cartesian(cartesian_mesh):
    assert cartesian_mesh.lower_boundary_condition ==  "periodic"
    assert cartesian_mesh.upper_boundary_condition ==  "periodic"
    assert cartesian_mesh.volume() == pytest.approx(10.)
    assert cartesian_mesh.volume(0) == pytest.approx(0.1)

def test_volume_spherical(spherical_mesh):
    assert spherical_mesh.lower_boundary_condition == "reflect"
    assert spherical_mesh.volume() == pytest.approx(4188.790204786391)
    assert spherical_mesh.volume(0) == pytest.approx(0.0041, abs = 1e-3)
