import flyft
import pytest
from pytest_lazy_fixtures import lf as lazy_fixture


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
    return flyft.state.CartesianMesh(2.0, 100, "periodic", 1)


@pytest.fixture
def spherical_mesh_sine():
    return flyft.state.SphericalMesh(2.0, 100, "zero")


@pytest.fixture(
    params=[lazy_fixture("cartesian_mesh_sine"), lazy_fixture("spherical_mesh_sine")]
)
def mesh_sine(request):
    return request.param


@pytest.fixture
def state_sine(mesh_sine):
    return flyft.State(flyft.state.ParallelMesh(mesh_sine), ("A",))


@pytest.fixture
def cartesian_mesh():
    return flyft.state.CartesianMesh(10.0, 100, "periodic", 1.0)


@pytest.fixture
def spherical_mesh():
    return flyft.state.SphericalMesh(10.0, 100, "repeat")


@pytest.fixture(params=[lazy_fixture("cartesian_mesh"), lazy_fixture("spherical_mesh")])
def mesh(request):
    return request.param


@pytest.fixture
def state(mesh):
    return flyft.State(flyft.state.ParallelMesh(mesh), ("A",))


@pytest.fixture
def binary_state(mesh):
    return flyft.State(flyft.state.ParallelMesh(mesh), ("A", "B"))


@pytest.fixture
def linear():
    return flyft.external.LinearPotential()


@pytest.fixture
def walls():
    return (flyft.external.HardWall(1.0, 1.0), flyft.external.HardWall(9.0, -1.0))


@pytest.fixture
def bd():
    return flyft.dynamics.BrownianDiffusiveFlux()


@pytest.fixture
def rpy():
    return flyft.dynamics.RPYDiffusiveFlux()
