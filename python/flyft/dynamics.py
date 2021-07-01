from . import _flyft
from . import mirror
from . import state

class Flux(mirror.Mirror,mirrorclass=_flyft.Flux):
    compute = mirror.Method()
    fluxes = mirror.WrappedProperty(state.Fields)

class CompositeFlux(Flux,mirror.CompositeMixin,mirrorclass=_flyft.CompositeFlux):
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

class CrankNicolsonIntegrator(Integrator,mirrorclass=_flyft.CrankNicolsonIntegrator):
    def __init__(self, timestep, mix_parameter, max_iterations, tolerance):
        super().__init__(timestep, mix_parameter, max_iterations, tolerance)

    timestep = mirror.Property()
    mix_parameter = mirror.Property()
    max_iterations = mirror.Property()
    tolerance = mirror.Property()

class ExplicitEulerIntegrator(Integrator,mirrorclass=_flyft.ExplicitEulerIntegrator):
    def __init__(self, timestep):
        super().__init__(timestep)

    timestep = mirror.Property()

class ImplicitEulerIntegrator(Integrator,mirrorclass=_flyft.ImplicitEulerIntegrator):
    def __init__(self, timestep, mix_parameter, max_iterations, tolerance):
        super().__init__(timestep, mix_parameter, max_iterations, tolerance)

    timestep = mirror.Property()
    mix_parameter = mirror.Property()
    max_iterations = mirror.Property()
    tolerance = mirror.Property()
