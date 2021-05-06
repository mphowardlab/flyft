from . import _flyft
from . import mirror

class CustomParameter(_flyft.DoubleParameter):
    pass

class _DoubleParameter(mirror.Mirror,mirrorclass=_flyft.DoubleParameter):
    pass
_DoubleParameter.mirror_method('__call__')

class LinearParameter(_DoubleParameter,mirrorclass=_flyft.LinearParameter):
    def __init__(self, initial, origin, rate):
        super().__init__(initial, origin, rate)
LinearParameter.mirror_property('initial')
LinearParameter.mirror_property('origin')
LinearParameter.mirror_property('rate')
