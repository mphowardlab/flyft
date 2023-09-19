from . import _flyft
from . import mirror

class CustomParameter(_flyft.DoubleParameter):
    pass

class _DoubleParameter(mirror.Mirror,mirrorclass=_flyft.DoubleParameter):
    __call__ = mirror.Method()

class LinearParameter(_DoubleParameter,mirrorclass=_flyft.LinearParameter):
    def __init__(self, initial, origin, rate):
        super().__init__(initial, origin, rate)

    initial = mirror.Property()
    origin = mirror.Property()
    rate = mirror.Property()

class SquareRootParameter(_DoubleParameter,mirrorclass=_flyft.SquareRootParameter):
    def __init__(self, initial, origin, rate):
        super().__init__(initial, origin, rate)

    initial = mirror.Property()
    origin = mirror.Property()
    rate = mirror.Property()
