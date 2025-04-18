import flyft
import numpy as np
import pytest


@pytest.fixture
def spherical_mesh_grand():
    return flyft.state.SphericalMesh(26.0, 1000, "repeat")


@pytest.fixture
def state_grand(spherical_mesh_grand):
    return flyft.State(flyft.state.ParallelMesh(spherical_mesh_grand), ("A",))


def test_ideal(grand, ig, rpy, state_grand):
    state = state_grand
    ai = 0.5
    R = state.mesh.full.L
    """The external field is determined is using a harmonic potential
    given by Vext = k * (x - R^2)
    where \beta k = 100 d^{-2}, droplet radius (R) = 25d.
    This term is differentiated w.r.t. x and multiplied by density field
    \rho(x) to generate the force produced. In this calculation, \rho(x) is
    assumed to be constant
    \rho(x) = 1
    Thus, ideal gas contribution to the total flux becomes 0.
    Using the above mentioned procedure, calculation can be done analytically
    for a given density field. The calculation of the integration of the
    mobility was done in Mathematica and the equations for the different
    limits of integration calculation are implemented below."""

    ig.volumes["A"] = 1.0
    rpy.diameters["A"] = 2 * ai
    rpy.viscosity = 1.0
    R_0 = 25

    Vext = flyft.external.HarmonicWall(R_0, -1.0)
    Vext.spring_constants["A"] = 100.0
    Vext.shifts["A"] = 0.0

    grand = flyft.functional.GrandPotential(ig, Vext)

    # same thing with all ones
    state.fields["A"][:] = 1
    grand.constrain("A", state.mesh.full.volume(), grand.Constraint.N)
    rpy.compute(grand, state)
    x = state.mesh.local.centers
    flux = np.zeros(len(x))
    lower_bounds = np.zeros_like(flux)

    for i in range(len(x)):
        left_edge = state.mesh.local.lower_bound(i)
        if left_edge < 2 * ai:
            BD = 0
            f = 0
        elif left_edge > 2 * ai and left_edge <= 24:
            BD = 0
            f = 0
        elif left_edge > 24 and left_edge <= 25:
            BD = 0
            f = -(
                0.868056
                * ((-24 + left_edge) ** 3)
                * (-3504 + left_edge * (-438 + (-8 + left_edge) * left_edge))
            ) / (left_edge**2)
        elif left_edge > 25 and left_edge < 26:
            BD = -0.106103 * (
                31250.0 + 100.0 * (-25.0 * left_edge + 0.5 * left_edge**2)
            )
            f = -(
                0.868056
                * ((-24 + left_edge) ** 3)
                * (-3504 + left_edge * (-438 + (-8 + left_edge) * left_edge))
            ) / (left_edge**2)
        else:
            BD = -0.106103 * (
                31250.0 + 100.0 * (-25.0 * left_edge + 0.5 * left_edge**2)
            )
            f = (138.889 + left_edge**2 * (-694.444 + 27.7778 * left_edge)) / (
                left_edge**2
            )

        # Multiplying rho(x) into M at the end
        flux[i] = f + BD
        lower_bounds[i] = left_edge
        print(lower_bounds[i], flux[i], rpy.fluxes["A"][i])
    flags = lower_bounds < R - (2 * ai)
    assert np.allclose(rpy.fluxes["A"][flags], flux[flags], atol=2e-2)
