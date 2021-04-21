from . import _flyft
from . import mirror
from . import state

class Functional(mirror.Mirror,mirrorclass=_flyft.Functional):
    @property
    def derivatives(self):
        if not hasattr(self, '_derivatives') or self._derivatives._self is not self._self.derivatives:
            self._derivatives = state.Fields(self._self.derivatives)
        return self._derivatives
Functional.mirror_property('value')
Functional.mirror_method('compute')

class Composite(Functional,mirrorclass=_flyft.CompositeFunctional):
    def __init__(self, functionals=None):
        super().__init__()
        self._functionals = []

        if functionals is not None:
            self.functionals = functionals

    @property
    def functionals(self):
        return self._functionals

    @functionals.setter
    def functionals(self, value):
        self._self.clear()
        self._functionals = []
        try:
            fs = list(value)
        except TypeError:
            fs = [value]
        self.extend(fs)

    def append(self, f):
        if f not in self._functionals:
            self._self.append(f._self)
            self._functionals.append(f)

    def remove(self, f):
        self._self.remove(f._self)
        self._functionals.remove(f)

    def extend(self, functionals):
        for f in functionals:
            self.append(f)

class IdealGas(Functional,mirrorclass=_flyft.IdealGasFunctional):
    pass
IdealGas.mirror_mapped_property('volumes')

class RosenfeldFMT(Functional,mirrorclass=_flyft.RosenfeldFMT):
    pass
RosenfeldFMT.mirror_mapped_property('diameters')

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
GrandPotential.mirror_property('ideal')
GrandPotential.mirror_property('excess')
GrandPotential.mirror_property('external')
GrandPotential.mirror_mapped_property('constraints')
GrandPotential.mirror_mapped_property('constraint_types')
