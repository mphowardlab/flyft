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

class ExplicitEulerIntegrator(Integrator,mirrorclass=_flyft.ExplicitEulerIntegrator):
    def __init__(self, timestep):
        super().__init__(timestep)

    timestep = mirror.Property()
