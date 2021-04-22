from . import _flyft
from .functional import Functional
from . import state

class ExternalPotential(Functional,mirrorclass=_flyft.ExternalPotential):
    pass

class Composite(ExternalPotential,mirrorclass=_flyft.CompositeExternalPotential):
    def __init__(self, potentials=None):
        super().__init__()
        self._potentials = []

        if potentials is not None:
            self.potentials = potentials

    @property
    def potentials(self):
        return self._potentials

    @potentials.setter
    def potentials(self, value):
        self._self.clear()
        self._potentials = []
        try:
            fs = list(value)
        except TypeError:
            fs = [value]
        self.extend(fs)

    def append(self, f):
        if f not in self._potentials:
            self._self.append(f._self)
            self._potentials.append(f)

    def remove(self, f):
        self._self.remove(f._self)
        self._potentials.remove(f)

    def extend(self, potentials):
        for f in potentials:
            self.append(f)

class WallPotential(ExternalPotential,mirrorclass=_flyft.WallPotential):
    def __init__(self, origin, normal):
        super().__init__(origin, normal)
WallPotential.mirror_property('origin')
WallPotential.mirror_property('normal')

class ExponentialWall(WallPotential,mirrorclass=_flyft.ExponentialWallPotential):
    pass
ExponentialWall.mirror_mapped_property('epsilons')
ExponentialWall.mirror_mapped_property('kappas')
ExponentialWall.mirror_mapped_property('shifts')

class HardWall(WallPotential,mirrorclass=_flyft.HardWallPotential):
    pass
HardWall.mirror_mapped_property('diameters')

class HarmonicWall(WallPotential,mirrorclass=_flyft.HarmonicWallPotential):
    pass
HarmonicWall.mirror_mapped_property('spring_constants')
HarmonicWall.mirror_mapped_property('shifts')

class LennardJones93Wall(WallPotential,mirrorclass=_flyft.LennardJones93WallPotential):
    pass
LennardJones93Wall.mirror_mapped_property('epsilons')
LennardJones93Wall.mirror_mapped_property('sigmas')
LennardJones93Wall.mirror_mapped_property('cutoffs')
LennardJones93Wall.mirror_mapped_property('shifts')
