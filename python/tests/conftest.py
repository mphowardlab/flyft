import pytest
import flyft





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
def state():
    mesh = flyft.state.ParallelMesh(flyft.state.CartesianMesh(10.0,20,1))
    return flyft.State(mesh,('A'))

@pytest.fixture
def binary_state():
    mesh = flyft.state.ParallelMesh(flyft.state.CartesianMesh(10.0,20,1))
    return flyft.State(mesh,('A','B'))

@pytest.fixture
def linear():
    return flyft.external.LinearPotential()

@pytest.fixture
def walls():
    return (flyft.external.HardWall(1.0,1.0),flyft.external.HardWall(9.0,-1.0))

@pytest.fixture
def bd():
    return flyft.dynamics.BrownianDiffusiveFlux()
