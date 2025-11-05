from . import _flyft, mirror
from .mixins import CompositeMixin, FixedPointAlgorithmMixin
from .state import Fields


class Flux(mirror.Mirror, mirrorclass=_flyft.Flux):
    """Base class for computing particle fluxes.

    `Flux` computes the flux of particles for different confining geometries. 
    The flux is used by integrators to update density fields over time.
    
    All flux classes must implement the `compute` method to calculate fluxes
    for all particle types in the system.
    """
    
    compute = mirror.Method()
    fluxes = mirror.WrappedProperty(Fields)


class CompositeFlux(Flux, CompositeMixin, mirrorclass=_flyft.CompositeFlux):
    """Composite flux combining multiple flux components.
    
    A `CompositeFlux` allows combining multiple flux objects into a single
    flux calculation.
    
    Parameters
    ----------
    objects : list, optional
        List of flux objects to combine. Can be set later using the `objects`
        attribute.
        
    Example
    -------
    Create a composite flux from multiple components:
    
    .. code-block:: python

        diffusive_flux_1 = flyft.dynamics.BrownianDiffusiveFlux()
        diffusive_flux_2 = flyft.dynamics.BrownianDiffusiveFlux()
        composite = flyft.dynamics.CompositeFlux([diffusive_flux_1, diffusive_flux_2])

    """
    
    def __init__(self, objects=None):
        super().__init__()
        if objects is not None:
            self.objects = objects


class BrownianDiffusiveFlux(Flux, mirrorclass=_flyft.BrownianDiffusiveFlux):
    """Brownian diffusive flux for particle transport.
    
    Computes particle flux due to Brownian diffusion, including both ideal
    (Fickian) diffusion and contributions from gradients in the excess chemical
    potential and external potentials. The flux is given by:
    
    .. math::
        \\mathbf{J} = -D \\left( \\nabla \\rho + \\rho \\nabla \\mu_{\\text{ex}} + \\rho \\nabla V \\right)
        
    where :math:`D` is the diffusivity, :math:`\\rho` is the density,
    :math:`\\mu_{\\text{ex}}` is the excess chemical potential, and :math:`V`
    is the external potential.
    
    Attributes
    ----------
    diffusivities : dict
        Dictionary mapping particle type names to their diffusivity values.
        
    Example
    -------
    Create a Brownian diffusive flux and set diffusivities:
    
    .. code-block:: python

        flux = flyft.dynamics.BrownianDiffusiveFlux()
        flux.diffusivities['A'] = 1.0
        flux.diffusivities['B'] = 0.5
        
    """
    
    diffusivities = mirror.WrappedProperty(mirror.MutableMapping)


class RPYDiffusiveFlux(Flux, mirrorclass=_flyft.RPYDiffusiveFlux):
    """Rotne-Prager-Yamakawa (RPY) diffusive flux with hydrodynamic interactions.
    
    Computes particle flux including hydrodynamic interactions between particles
    using the Rotne-Prager-Yamakawa mobility tensor for hard spheres. For particles
    i and j separated by distance r > d (where d = (a_i + a_j)):
    
    .. math::
        \\mathbf{M}_{ij} = \\frac{1}{8\\pi\\eta r} \\left[\\left(1 + \\frac{a_i^2+a_j^2}{3r^2}\\right)\\mathbf{I} 
                                                          + \\left(1 - \\frac{a_i^2+ a_j^2}{r^2}\\right)\\hat{\\mathbf{r}}\\hat{\\mathbf{r}} \\right]
        
    where :math:`\\eta` is the fluid viscosity, :math:`a_i` and :math:`a_j` are the 
    particle radii, :math:`r` is the distance between particles, :math:`\\mathbf{I}` 
    is the identity tensor, and :math:`\\hat{\\mathbf{r}}` is the unit vector connecting 
    the particle centers.
    
    Attributes
    ----------
    diameters : dict
        Dictionary mapping particle type names to their diameters.
    viscosity : float
        Fluid viscosity.
        
    Example
    -------
    Create an RPY diffusive flux and set parameters:
    
    .. code-block:: python
    
        flux = flyft.dynamics.RPYDiffusiveFlux()
        flux.diameters['A'] = 1.0
        flux.diameters['B'] = 2.0
        flux.viscosity = 1.0
        
    """
    
    diameters = mirror.WrappedProperty(mirror.MutableMapping)
    viscosity = mirror.Property()


class Integrator(mirror.Mirror, mirrorclass=_flyft.Integrator):
    """Base class for time integration of density fields.
    
    An `Integrator` advances the system state in time by applying fluxes
    computed from the grand potential and current state. It supports both
    fixed and adaptive timestep schemes for stability and efficiency.
    
    All integrator classes must implement specific integration schemes
    (explicit, implicit, etc.) while sharing common functionality for
    timestep management and adaptive stepping.
    
    Attributes
    ----------
    timestep : float
        Integration timestep.
    adaptive : bool
        Whether to use adaptive timestep control.
    adapt_delay : float
        Time delay before starting adaptive timestep control.
    adapt_tolerance : float
        Error tolerance for adaptive timestep control.
    adapt_minimum : float
        Minimum allowed timestep for adaptive control.
    """
    
    advance = mirror.Method()
    timestep = mirror.Property()
    adaptive = mirror.Property()
    adapt_delay = mirror.Property()
    adapt_tolerance = mirror.Property()
    adapt_minimum = mirror.Property()

    def use_adaptive(self, delay=0, tolerance=1.0e-8, minimum=1.0e-8):
        """Enable adaptive timestep control.
        
        Configures the integrator to use adaptive timestep control based on
        error estimation. The timestep is automatically adjusted to maintain
        the specified error tolerance.
        
        Parameters
        ----------
        delay : float, optional
            Time delay before adaptive control begins. Default is 0.
        tolerance : float, optional
            Error tolerance for timestep adjustment. Default is 1e-8.
        minimum : float, optional
            Minimum allowed timestep. Default is 1e-8.
            
        Example
        -------
        Enable adaptive timestep control:
        
        .. code-block:: python
        
            integrator.use_adaptive(delay=10.0, tolerance=1e-6, minimum=1e-10)
            
        """
        self.adaptive = True
        self.adapt_delay = delay
        self.adapt_tolerance = tolerance
        self.adapt_minimum = minimum


class CrankNicolsonIntegrator(
    Integrator, FixedPointAlgorithmMixin, mirrorclass=_flyft.CrankNicolsonIntegrator
):
    """Crank-Nicolson time integrator with second-order accuracy.
    
    Implements the Crank-Nicolson scheme, which is second-order accurate in time
    and unconditionally stable. The scheme uses an implicit trapezoidal rule
    that averages the flux at the current and next timesteps:
    
    .. math::
        \\rho^{n+1} = \\rho^n + \\frac{\\Delta t}{2} \\left( F[\\rho^n] + F[\\rho^{n+1}] \\right)
        
    where :math:`F[\\rho]` represents the flux evaluation. Since this is an
    implicit method, it requires iterative solution at each timestep.
    
    Parameters
    ----------
    timestep : float
        Integration timestep.
    mix_parameter : float
        Mixing parameter for fixed-point iteration convergence.
    max_iterations : int
        Maximum number of iterations for implicit solve.
    tolerance : float
        Convergence tolerance for implicit solve.
        
    Example
    -------
    Create a Crank-Nicolson integrator:
    
    .. code-block:: python

        integrator = flyft.dynamics.CrankNicolsonIntegrator(
            timestep=0.01,
            mix_parameter=0.1,
            max_iterations=100,
            tolerance=1e-8
        )
        
    """
    
    def __init__(self, timestep, mix_parameter, max_iterations, tolerance):
        super().__init__(timestep, mix_parameter, max_iterations, tolerance)


class ExplicitEulerIntegrator(Integrator, mirrorclass=_flyft.ExplicitEulerIntegrator):
    """Explicit Euler time integrator with first-order accuracy.
    
    Implements the explicit (forward) Euler method, which is first-order
    accurate in time and conditionally stable. The scheme uses the current
    flux to advance the density:
    
    .. math::
        \\rho^{n+1} = \\rho^n + \\Delta t \\, F[\\rho^n]
        
    where :math:`F[\\rho]` represents the flux evaluation. This explicit
    method is simple and efficient but requires small timesteps for stability.
    
    Parameters
    ----------
    timestep : float
        Integration timestep. Must be sufficiently small for stability.
        
    Example
    -------
    Create an explicit Euler integrator:
    
    .. code-block:: python

        integrator = flyft.dynamics.ExplicitEulerIntegrator(timestep=0.001)

    Note
    ----
    The explicit Euler method has stability constraints that depend on the
    diffusivity and mesh spacing. Typically, the timestep must satisfy
    :math:`\\Delta t < \\Delta x^2 / (2D)` where :math:`\\Delta x` is the
    mesh spacing and :math:`D` is the diffusivity.
    """
    
    def __init__(self, timestep):
        super().__init__(timestep)


class ImplicitEulerIntegrator(
    Integrator, FixedPointAlgorithmMixin, mirrorclass=_flyft.ImplicitEulerIntegrator
):
    """Implicit Euler time integrator with first-order accuracy.
    
    Implements the implicit (backward) Euler method, which is first-order
    accurate in time and unconditionally stable. The scheme uses the flux
    at the next timestep to advance the density:
    
    .. math::
        \\rho^{n+1} = \\rho^n + \\Delta t \\, F[\\rho^{n+1}]
        
    where :math:`F[\\rho]` represents the flux evaluation. Since this is an
    implicit method, it requires iterative solution at each timestep but
    offers superior stability compared to explicit methods.
    
    Parameters
    ----------
    timestep : float
        Integration timestep.
    mix_parameter : float
        Mixing parameter for fixed-point iteration convergence.
    max_iterations : int
        Maximum number of iterations for implicit solve.
    tolerance : float
        Convergence tolerance for implicit solve.
        
    Example
    -------
    Create an implicit Euler integrator:
    
    .. code-block:: python

        integrator = flyft.dynamics.ImplicitEulerIntegrator(
            timestep=0.1,
            mix_parameter=0.1,
            max_iterations=100,
            tolerance=1e-8
        )
        
    Note
    ----
    The implicit Euler method is unconditionally stable, allowing larger
    timesteps than explicit methods. However, it requires solving a nonlinear
    system at each timestep, which can be computationally expensive.
    """
    
    def __init__(self, timestep, mix_parameter, max_iterations, tolerance):
        super().__init__(timestep, mix_parameter, max_iterations, tolerance)
