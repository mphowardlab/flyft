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

class DiffusiveFlux(Flux,mirrorclass=_flyft.DiffusiveFlux):
    pass
DiffusiveFlux.mirror_mapped_property('diffusivities')

class BrownianDiffusiveFlux(DiffusiveFlux,mirrorclass=_flyft.BrownianDiffusiveFlux):
    pass
