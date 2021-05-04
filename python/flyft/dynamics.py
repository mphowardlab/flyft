from . import _flyft
from . import mirror
from . import state

class Flux(mirror.Mirror,mirrorclass=_flyft.Flux):
    @property
    def fluxes(self):
        if not hasattr(self, '_fluxes') or self._fluxes._self is not self._self.fluxes:
            self._fluxes = state.Fields(self._self.fluxes)
        return self._fluxes
Flux.mirror_method('compute')

class CompositeFlux(Flux,mirror.CompositeMixin,mirrorclass=_flyft.CompositeFlux):
    def __init__(self, objects=None):
        super().__init__()
        if objects is not None:
            self.objects = objects

class DiffusiveFlux(Flux,mirrorclass=_flyft.DiffusiveFlux):
    pass
DiffusiveFlux.mirror_mapped_property('diffusivities')

class BrownianDiffusiveFlux(DiffusiveFlux,mirrorclass=_flyft.BrownianDiffusiveFlux):
    pass

class Integrator(mirror.Mirror,mirrorclass=_flyft.Integrator):
    pass
Integrator.mirror_method('advance')

class ExplicitEulerIntegrator(Integrator,mirrorclass=_flyft.ExplicitEulerIntegrator):
    def __init__(self, timestep):
        super().__init__(timestep)
ExplicitEulerIntegrator.mirror_property('timestep')
