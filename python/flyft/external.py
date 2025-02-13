from . import _flyft, mirror
from .functional import Functional
from .mixins import CompositeMixin


class ExternalPotential(Functional, mirrorclass=_flyft.ExternalPotential):
    pass


class CompositeExternalPotential(
    ExternalPotential, CompositeMixin, mirrorclass=_flyft.CompositeExternalPotential
):
    def __init__(self, objects=None):
        super().__init__()
        if objects is not None:
            self.objects = objects


class LinearPotential(ExternalPotential, mirrorclass=_flyft.LinearPotential):
    def set_line(self, type_, x, y, slope):
        self.xs[type_] = x
        self.ys[type_] = y
        self.slopes[type_] = slope

    xs = mirror.WrappedProperty(mirror.MutableMapping)
    ys = mirror.WrappedProperty(mirror.MutableMapping)
    slopes = mirror.WrappedProperty(mirror.MutableMapping)


class WallPotential(ExternalPotential):
    def __init__(self, origin, normal):
        super().__init__(origin, normal)
        # force a second time to cache if needed
        self.origin = origin
        self.normal = normal

    origin = mirror.Property()
    normal = mirror.Property()


class ExponentialWall(WallPotential, mirrorclass=_flyft.ExponentialWallPotential):
    epsilons = mirror.WrappedProperty(mirror.MutableMapping)
    kappas = mirror.WrappedProperty(mirror.MutableMapping)
    shifts = mirror.WrappedProperty(mirror.MutableMapping)


class HardWall(WallPotential, mirrorclass=_flyft.HardWallPotential):
    diameters = mirror.WrappedProperty(mirror.MutableMapping)


class HarmonicWall(WallPotential, mirrorclass=_flyft.HarmonicWallPotential):
    spring_constants = mirror.WrappedProperty(mirror.MutableMapping)
    shifts = mirror.WrappedProperty(mirror.MutableMapping)


class LennardJones93Wall(WallPotential, mirrorclass=_flyft.LennardJones93WallPotential):
    epsilons = mirror.WrappedProperty(mirror.MutableMapping)
    sigmas = mirror.WrappedProperty(mirror.MutableMapping)
    cutoffs = mirror.WrappedProperty(mirror.MutableMapping)
    shifts = mirror.WrappedProperty(mirror.MutableMapping)
