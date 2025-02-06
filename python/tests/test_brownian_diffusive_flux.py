import numpy as np
import pytest
from pytest_lazy_fixtures import lf as lazy_fixture

import flyft


@pytest.fixture
def cartesian_mesh_grand():
    return flyft.state.CartesianMesh(10.0, 100, "periodic", 1)


@pytest.fixture
def spherical_mesh_grand():
    return flyft.state.SphericalMesh(10.0, 100, "repeat")


@pytest.fixture(
    params=[lazy_fixture("cartesian_mesh_grand"), lazy_fixture("spherical_mesh_grand")]
)
def mesh_grand(request):
    return request.param


@pytest.fixture
def state_grand(mesh_grand):
    return flyft.State(flyft.state.ParallelMesh(mesh_grand), ("A",))


def test_diffusivities(bd):
    assert len(bd.diffusivities) == 0

    # set by key
    bd.diffusivities["A"] = 1.0
    assert bd.diffusivities["A"] == pytest.approx(1.0)
    assert bd._self.diffusivities["A"] == pytest.approx(1.0)

    # set by update
    bd.diffusivities.update(B=2.0)
    assert bd.diffusivities["A"] == pytest.approx(1.0)
    assert bd.diffusivities["B"] == pytest.approx(2.0)

    # set as new dict
    bd.diffusivities = {"A": 1.5, "B": 2.5}
    assert bd.diffusivities["A"] == pytest.approx(1.5)
    assert bd.diffusivities["B"] == pytest.approx(2.5)

    # check mirror class values
    assert bd._self.diffusivities["A"] == pytest.approx(1.5)
    assert bd._self.diffusivities["B"] == pytest.approx(2.5)


def test_ideal(grand, ig, bd, state_grand):
    state = state_grand

    volume = state.mesh.full.volume()
    ig.volumes["A"] = 1.0
    grand.ideal = ig
    bd.diffusivities["A"] = 2.0

    # first check OK with all zeros
    state.fields["A"][:] = 0.0
    grand.constrain("A", 0, grand.Constraint.N)
    bd.compute(grand, state)
    assert np.allclose(bd.fluxes["A"], 0.0)

    # same thing with all ones
    state.fields["A"][:] = 1.0
    grand.constrain("A", volume, grand.Constraint.N)
    bd.compute(grand, state)
    assert np.allclose(bd.fluxes["A"], 0.0)
    x = state.mesh.local.centers
    state.fields["A"][:] = 3.0 / state.mesh.full.L * x
    grand.constrain("A", state.mesh.full.L * 1.5, grand.Constraint.N)
    bd.compute(grand, state)
    assert np.allclose(bd.fluxes["A"][1:], -2.0 * 3.0 / state.mesh.full.L)
    # make linear profile and test in the middle
    # (skip first one because there is jump over PBC)
    # j = -D (drho/dx)
    if isinstance(state.mesh.full, flyft.state.CartesianMesh):
        # make sinusoidal profile and test with looser tolerance due to finite diff
        state.fields["A"][:] = np.sin(2 * np.pi * x / state.mesh.full.L) + 1
        grand.constrain("A", volume, grand.Constraint.N)
        bd.compute(grand, state)
        # flux is computed at the left edge
        j = (
            -2.0
            * (2.0 * np.pi / state.mesh.full.L)
            * np.cos(2 * np.pi * (x - 0.5 * state.mesh.full.step) / state.mesh.full.L)
        )
        assert np.allclose(bd.fluxes["A"], j, rtol=1e-3, atol=1e-3)


def test_excess(grand, state_grand, ig, bd):
    state = state_grand

    virial = flyft.functional.VirialExpansion()
    B = 5.0

    ig.volumes["A"] = 1.0
    virial.coefficients["A", "A"] = B
    grand.ideal = ig
    grand.excess = virial
    grand.constrain("A", state.mesh.full.L * 1.0, grand.Constraint.N)
    bd.diffusivities["A"] = 2.0

    # first check OK with all zeros
    state.fields["A"][:] = 0.0
    grand.constrain("A", 0, grand.Constraint.N)
    bd.compute(grand, state)
    assert np.allclose(bd.fluxes["A"], 0.0)

    # same thing with all constant
    state.fields["A"][:] = 1.0e-2
    grand.constrain("A", state.mesh.full.L * 1.0e-2, grand.Constraint.N)
    bd.compute(grand, state)
    assert np.allclose(bd.fluxes["A"], 0.0)
    # TODO: replace this with a local hs functional instead of fmt so the test is exact
    if isinstance(state.mesh.full, flyft.state.CartesianMesh):
        x = state.mesh.local.centers
        state.fields["A"][:] = 1.0e-1 * (np.sin(2 * np.pi * x / state.mesh.full.L) + 1)
        grand.constrain("A", state.mesh.full.L * 1.0e-1, grand.Constraint.N)
        bd.compute(grand, state)
        # flux is computed at the left edge
        rho = 1.0e-1 * (
            np.sin(2 * np.pi * (x - 0.5 * state.mesh.full.step) / state.mesh.full.L) + 1
        )
        drho_dx = (
            1.0e-1
            * (2.0 * np.pi / state.mesh.full.L)
            * np.cos(2 * np.pi * (x - 0.5 * state.mesh.full.step) / state.mesh.full.L)
        )
        dmuex_drho = 2 * B
        jid = -2.0 * drho_dx
        jex = -2.0 * rho * dmuex_drho * drho_dx
        j = jid + jex
        assert np.allclose(bd.fluxes["A"], j, rtol=1e-3, atol=1e-3)


def test_external(state, grand, ig, walls, linear, bd):
    ig.volumes["A"] = 1.0
    grand.ideal = ig
    for w in walls:
        w.diameters["A"] = 0.0
    grand.external = flyft.external.CompositeExternalPotential(walls)
    bd.diffusivities["A"] = 2.0

    # first check OK with all zeros
    state.fields["A"][:] = 0.0
    grand.constrain("A", 0, grand.Constraint.N)
    bd.compute(grand, state)
    assert np.allclose(bd.fluxes["A"], 0.0)

    # all 1s outside the walls (should give no flux into walls still)
    x = state.mesh.local.centers
    inside = np.logical_and(x > walls[0].origin, x < walls[1].origin)
    state.fields["A"][inside] = 1.0
    grand.constrain(
        "A", np.sum(state.fields["A"]) * state.mesh.full.step, grand.Constraint.N
    )
    bd.compute(grand, state)
    assert np.allclose(bd.fluxes["A"], 0.0)

    # try linear gradient between walls
    slope = (2.0 - 1.0) / (walls[1].origin - walls[0].origin)
    state.fields["A"][inside] = 1.0 + slope * (x[inside] - walls[0].origin)
    grand.constrain(
        "A", np.sum(state.fields["A"]) * state.mesh.full.step, grand.Constraint.N
    )
    bd.compute(grand, state)
    lower = np.array(
        [state.mesh.local.lower_bound(i) for i in range(state.mesh.local.shape)]
    )
    flags = np.logical_and(lower > walls[0].origin, lower < walls[1].origin)
    assert np.allclose(bd.fluxes["A"][flags], -2.0 * slope)

    # use linear potential to check gradient calculation between walls
    state.fields["A"][inside] = 3.0
    grand.constrain(
        "A", np.sum(state.fields["A"]) * state.mesh.full.step, grand.Constraint.N
    )
    linear.set_line("A", x=walls[0].origin, y=0.0, slope=0.25)
    grand.external.append(linear)
    bd.compute(grand, state)
    assert np.allclose(bd.fluxes["A"][flags], 3.0 * 2.0 * -0.25)
