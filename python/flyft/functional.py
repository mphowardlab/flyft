from . import _flyft, mirror
from .mixins import CompositeMixin
from .state import Fields


class Functional(mirror.Mirror, mirrorclass=_flyft.Functional):
    r"""Base class for free energy functionals in density functional theory.

    `Functional` computes the free energy contribution and its functional
    derivatives with respect to the density fields.

    All functionals implement the `compute` method to evaluate both the total
    free energy value and the derivatives (chemical potential contributions)
    for all particle types in the system.

    Attributes
    ----------
    derivatives : Fields
        Functional derivatives (chemical potential contributions) for each
        particle type.
    value : float
        Total free energy contribution from this functional.
    """

    derivatives = mirror.WrappedProperty(Fields)
    value = mirror.Property()

    _compute = mirror.Method(mirrorname="compute")

    def compute(self, state, value=True):
        """Compute the functional and its derivatives.

        Parameters
        ----------
        state : State
            System state containing density fields and mesh information.
        value : bool, optional
            Whether to compute the total free energy value. Default is True.
        """
        self._compute(state, value)


class BoublikHardSphere(Functional, mirrorclass=_flyft.BoublikHardSphereFunctional):
    r"""BMCSL hard sphere functional for excess free energy of hard spheres
    systems.

    Implements the BMCSL equation of state for hard sphere mixtures using
    local density approximations (LDA).

    Attributes
    ----------
    diameters : dict
        Dictionary mapping particle type names to their hard sphere diameters.

    Example
    -------
    Apply BMCSL hard sphere free-energy functional:

    .. code-block:: python

        functional = flyft.functional.BoublikHardSphere()
        functional.diameters['A'] = 1.0
        functional.diameters['B'] = 1.5

    """

    diameters = mirror.WrappedProperty(mirror.MutableMapping)


class CompositeFunctional(
    Functional, CompositeMixin, mirrorclass=_flyft.CompositeFunctional
):
    r"""Composite functional combining multiple functional components.

    `CompositeFunctional` allows combining multiple functional objects
    into a single functional calculation.

    The total free energy is the sum of all component functionals, and
    derivatives are computed by summing the individual contributions.

    Parameters
    ----------
    objects : list, optional
        List of functional objects to combine. Can be set later using
        the `objects` attribute.

    Example
    -------
    Create a composite functional from multiple components:

    .. code-block:: python

        ideal = flyft.functional.IdealGas()
        hard_sphere = flyft.functional.WhiteBear()
        composite = flyft.CompositeFunctional([ideal, hard_sphere])

    """

    def __init__(self, objects=None):
        super().__init__()
        if objects is not None:
            self.objects = objects


class GrandPotential(Functional, mirrorclass=_flyft.GrandPotential):
    """Grand potential functional for density functional theory calculations.

    The `GrandPotential` combines ideal gas, excess, and external potential
    contributions to form the complete grand potential functional:

    .. math::
        \\Omega[\\rho] = F_{\\text{ideal}}[\\rho] + F_{\\text{excess}}[\\rho]
                        + \\int V_{\\text{ext}}(\\mathbf{r})
                        \\rho(\\mathbf{r}) d\\mathbf{r}

    where :math:`F_{\\text{ideal}}` is the ideal gas contribution,
    :math:`F_{\\text{excess}}` represents particle interactions, and
    :math:`V_{\\text{ext}}` is the external potential.


    Parameters
    ----------
    ideal : Functional, optional
        Ideal gas functional component.
    excess : Functional, optional
        Excess free energy functional for particle interactions.
    external : ExternalPotential, optional
        External potential component.

    Attributes
    ----------
    ideal : Functional
        Ideal gas functional.
    excess : Functional
        Excess interaction functional.
    external : ExternalPotential
        External potential.
    constraints : dict
        Constraint values for different particle types.
    constraint_types : dict
        Types of constraints applied to each particle type.

    Example
    -------
    Create a grand potential functional:

    .. code-block:: python

        ideal = flyft.functional.IdealGas()
        excess = flyft.functional.WhiteBear()
        external = flyft.external.HardWall(origin=0.0, normal=1.0)
        grand = flyft.functional.GrandPotential(ideal=ideal, excess=excess,
                                                external=external)

    """

    def __init__(self, ideal=None, excess=None, external=None):
        super().__init__()
        if ideal is not None:
            self.ideal = ideal
        if excess is not None:
            self.excess = excess
        if external is not None:
            self.external = external

    Constraint = _flyft.GrandPotential.Constraint

    def constrain(self, key, value, constraint_type):
        """Apply a constraint to a particle type.

        Parameters
        ----------
        key : str
            Particle type name.
        value : float
            Constraint value.
        constraint_type : Constraint
            Type of constraint to apply.
        """
        self.constraints[key] = value
        self.constraint_types[key] = constraint_type

    ideal = mirror.Property()
    excess = mirror.Property()
    external = mirror.Property()
    constraints = mirror.WrappedProperty(mirror.MutableMapping)
    constraint_types = mirror.WrappedProperty(mirror.MutableMapping)


class IdealGas(Functional, mirrorclass=_flyft.IdealGasFunctional):
    r"""Ideal gas functional for non-interacting particles.

    Implements the ideal gas free energy functional:

    .. math::

        \mathcal{F}_{\mathrm{ideal}}[\rho] = k_B T \int \rho(\mathbf{r}) \left[
                                \ln(\rho(\mathbf{r}) v) - 1 \right]
                                d\mathbf{r}

    where :math:`\rho(\mathbf{r})` is the density field, :math:`v` is the
    molecular volume, :math:`k_B` is Boltzmann's constant, and :math:`T`
    is temperature.

    This functional provides the entropic contribution for non-interacting
    particles and is typically combined with excess functionals to describe
    interacting systems.

    Attributes
    ----------
    volumes : dict
        Dictionary mapping particle type names to their molecular volumes.

    Example
    -------
    Create an ideal gas functional:

    .. code-block:: python

        ideal = flyft.functional.IdealGas()
        ideal.volumes['A'] = 1.0
        ideal.volumes['B'] = 1.5

    """

    volumes = mirror.WrappedProperty(mirror.MutableMapping)


class RosenfeldFMT(Functional, mirrorclass=_flyft.RosenfeldFMT):
    r"""Rosenfeld fundamental measure theory functional for hard spheres.

    Implements the original Rosenfeld fundamental measure theory (FMT)
    functional for hard sphere systems. This functional provides an
    accurate description of hard sphere correlations and excluded volume
    effects using weighted densities based on fundamental measures.


    Attributes
    ----------
    diameters : dict
        Dictionary mapping particle type names to their hard sphere diameters.

    Example
    -------
    Create a Rosenfeld FMT functional:

    .. code-block:: python

        fmt = flyft.functional.RosenfeldFMT()
        fmt.diameters['A'] = 1.0
        fmt.diameters['B'] = 1.2

    """

    diameters = mirror.WrappedProperty(mirror.MutableMapping)


class VirialExpansion(Functional, mirrorclass=_flyft.VirialExpansion):
    r"""Virial expansion functional for weakly interacting systems.

    Implements a virial expansion functional that approximates the excess
    free energy using virial coefficients. In this case the second order virial
    expansion is considered:

    .. math::

        \mathcal{F}^{\mathrm{ex}}(\rho) = k_B T\,4 \eta

    where, :math:`\eta` is the volume fraction of the system.

    Attributes
    ----------
    coefficients : dict
        Dictionary mapping particle type pairs to virial coefficient values.
        Keys should be tuples like ('A', 'B') for cross-interactions.

    Example
    -------
    Create a virial expansion functional:

    .. code-block:: python

        virial = flyft.functional.VirialExpansion()
        virial.coefficients[('A', 'A')] = -0.5  # Second virial coefficient
        virial.coefficients[('A', 'B')] = -0.3

    """

    coefficients = mirror.WrappedProperty(mirror.MutableMapping)


class WhiteBear(RosenfeldFMT, mirrorclass=_flyft.WhiteBear):
    """White Bear fundamental measure theory functional for hard spheres.

    Implements the White Bear version of fundamental measure theory (FMT),
    which is an variation of the Rosenfeld FMT functional.

    Example
    -------
    Create a White Bear FMT functional:

    .. code-block:: python

        fmt = flyft.functional.WhiteBear()
        fmt.diameters['A'] = 1.0
        fmt.diameters['B'] = 1.2

    """

    pass


class WhiteBearMarkII(WhiteBear, mirrorclass=_flyft.WhiteBearMarkII):
    """White Bear Mark II fundamental measure theory functional.

    Implements the White Bear Mark II version of fundamental measure theory,
    which is another variation of the White Bear functional.

    Example
    -------
    Create a White Bear Mark II FMT functional:

    .. code-block:: python

        fmt = flyft.functional.WhiteBearMarkII()
        fmt.diameters['A'] = 1.0
        fmt.diameters['B'] = 1.2

    """

    pass
