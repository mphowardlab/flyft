from . import _flyft
from . import mirror
from . import state

class Solver(mirror.Mirror,mirrorclass=_flyft.Solver):
    pass
Solver.mirror_method('solve')

class PiccardIteration(Solver,mirrorclass=_flyft.PiccardIteration):
    def __init__(self, mix_parameter, max_iterations, tolerance):
        super().__init__(mix_parameter, max_iterations, tolerance)
PiccardIteration.mirror_property('mix_parameter')
PiccardIteration.mirror_property('max_iterations')
PiccardIteration.mirror_property('tolerance')
