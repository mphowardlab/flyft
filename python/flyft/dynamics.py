from . import _flyft
from . import mirror
from .mixins import (CompositeMixin,FixedPointAlgorithmMixin)
from .state import Fields

class Flux(mirror.Mirror,mirrorclass=_flyft.Flux):
    compute = mirror.Method()
    fluxes = mirror.WrappedProperty(Fields)

class CompositeFlux(Flux,CompositeMixin,mirrorclass=_flyft.CompositeFlux):
    def __init__(self, objects=None):
        super().__init__()
        if objects is not None:
            self.objects = objects

class DiffusiveFlux(Flux,mirrorclass=_flyft.DiffusiveFlux):
    diffusivities = mirror.WrappedProperty(mirror.MutableMapping)

class BrownianDiffusiveFlux(DiffusiveFlux,mirrorclass=_flyft.BrownianDiffusiveFlux):
    pass

class Integrator(mirror.Mirror,mirrorclass=_flyft.Integrator):
    advance = mirror.Method()
    timestep = mirror.Property()
    adaptive = mirror.Property()
    adapt_delay = mirror.Property()
    adapt_tolerance = mirror.Property()
    adapt_minimum = mirror.Property()

    def use_adaptive(self, delay=0, tolerance=1.e-8, minimum=1.e-8):
        self.adaptive = True
        self.adapt_delay = delay
        self.adapt_tolerance = tolerance
        self.adapt_minimum = minimum

class CrankNicolsonIntegrator(Integrator,FixedPointAlgorithmMixin,mirrorclass=_flyft.CrankNicolsonIntegrator):
    def __init__(self, timestep, mix_parameter, max_iterations, tolerance):
        super().__init__(timestep, mix_parameter, max_iterations, tolerance)

class ExplicitEulerIntegrator(Integrator,mirrorclass=_flyft.ExplicitEulerIntegrator):
    def __init__(self, timestep):
        super().__init__(timestep)

class ImplicitEulerIntegrator(Integrator,FixedPointAlgorithmMixin,mirrorclass=_flyft.ImplicitEulerIntegrator):
    def __init__(self, timestep, mix_parameter, max_iterations, tolerance):
        super().__init__(timestep, mix_parameter, max_iterations, tolerance)
