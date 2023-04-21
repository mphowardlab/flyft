import pytest
import flyft
from pytest_lazyfixture import lazy_fixture

@pytest.fixture
def ig():
    return flyft.functional.IdealGas()

@pytest.fixture
def fmt():
    return flyft.functional.RosenfeldFMT()

@pytest.fixture
def grand():
    return flyft.functional.GrandPotential()

@pytest.fixture
def cartesian_mesh_sine():
    return flyft.state.CartesianMesh(2.,100,1)

@pytest.fixture
def spherical_mesh_sine():
    return flyft.state.SphericalMesh(2.,100)

@pytest.fixture(params=[lazy_fixture("cartesian_mesh_sine"), lazy_fixture("spherical_mesh_sine")])
def mesh_sine(request):
    return request.param

@pytest.fixture
def state_sine(mesh_sine):
    return flyft.State(flyft.state.ParallelMesh(mesh_sine), ("A", ))

@pytest.fixture
def cartesian_mesh():
    return flyft.state.CartesianMesh(10.0,20,1)

@pytest.fixture
def spherical_mesh():
    return flyft.state.SphericalMesh(10.0,20)
    
@pytest.fixture(params=[lazy_fixture("cartesian_mesh"), lazy_fixture("spherical_mesh")])
def mesh(request):
    return request.param
    
@pytest.fixture
def state(mesh):
    return flyft.State(flyft.state.ParallelMesh(mesh), ("A", ))

@pytest.fixture
def binary_state(mesh):
    return flyft.State(flyft.state.ParallelMesh(mesh), ("A", "B"))

@pytest.fixture
def linear():
    return flyft.external.LinearPotential()

@pytest.fixture
def walls():
    return (flyft.external.HardWall(1.0,1.0),flyft.external.HardWall(9.0,-1.0))

@pytest.fixture
def bd():
    return flyft.dynamics.BrownianDiffusiveFlux()
