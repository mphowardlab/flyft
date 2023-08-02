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
    periodic = flyft._flyft.BoundaryType.periodic
    return flyft.state.CartesianMesh(0,2.,100,"periodic",1)
    

@pytest.fixture
def spherical_mesh_sine():
    return flyft.state.SphericalMesh(0,2.,100,"reflect")


@pytest.fixture(params=[lazy_fixture("cartesian_mesh_sine"), lazy_fixture("spherical_mesh_sine")])
def mesh_sine(request):
    return request.param

@pytest.fixture
def state_sine(mesh_sine):
    return flyft.State(flyft.state.ParallelMesh(mesh_sine), ("A", ))

@pytest.fixture
def cartesian_mesh():
    return flyft.state.CartesianMesh(0.,10.0,20,"periodic",1.)

@pytest.fixture
def spherical_mesh():
    return flyft.state.SphericalMesh(0.,10.0,20,"reflect")
    
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
