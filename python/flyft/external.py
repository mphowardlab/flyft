from . import _flyft
from .functional import Functional
from . import mirror
from . import state

class ExternalPotential(Functional,mirrorclass=_flyft.ExternalPotential):
    pass

class CompositeExternalPotential(ExternalPotential,mirror.CompositeMixin,mirrorclass=_flyft.CompositeExternalPotential):
    def __init__(self, objects=None):
        super().__init__()
        if objects is not None:
            self.objects = objects

class LinearPotential(ExternalPotential,mirrorclass=_flyft.LinearPotential):
    pass
LinearPotential.mirror_mapped_property('xs')
LinearPotential.mirror_mapped_property('ys')
LinearPotential.mirror_mapped_property('slopes')

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
