from . import _flyft
from . import mirror
from .mixins import CompositeMixin
from .state import Fields

class Functional(mirror.Mirror,mirrorclass=_flyft.Functional):
    derivatives = mirror.WrappedProperty(Fields)
    value = mirror.Property()

    _compute = mirror.Method(mirrorname="compute")
    def compute(self, state, value=True):
        self._compute(state,value)

class BoublikHardSphere(Functional,mirrorclass=_flyft.BoublikHardSphereFunctional):
    diameters = mirror.WrappedProperty(mirror.MutableMapping)

class CompositeFunctional(Functional,CompositeMixin,mirrorclass=_flyft.CompositeFunctional):
    def __init__(self, objects=None):
        super().__init__()
        if objects is not None:
            self.objects = objects

class GrandPotential(Functional,mirrorclass=_flyft.GrandPotential):
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
        self.constraints[key] = value
        self.constraint_types[key] = constraint_type

    ideal = mirror.Property()
    excess = mirror.Property()
    external = mirror.Property()
    constraints = mirror.WrappedProperty(mirror.MutableMapping)
    constraint_types = mirror.WrappedProperty(mirror.MutableMapping)

class IdealGas(Functional,mirrorclass=_flyft.IdealGasFunctional):
    volumes = mirror.WrappedProperty(mirror.MutableMapping)

class RosenfeldFMT(Functional,mirrorclass=_flyft.RosenfeldFMT):
    diameters = mirror.WrappedProperty(mirror.MutableMapping)

class VirialExpansion(Functional,mirrorclass=_flyft.VirialExpansion):
    coefficients = mirror.WrappedProperty(mirror.MutableMapping)
