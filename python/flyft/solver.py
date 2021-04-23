from . import _flyft
from . import mirror
from . import state

class Solver(mirror.Mirror,mirrorclass=_flyft.Solver):
    pass
Solver.mirror_method('solve')

class PicardIteration(Solver,mirrorclass=_flyft.PicardIteration):
    def __init__(self, mix_parameter, max_iterations, tolerance):
        super().__init__(mix_parameter, max_iterations, tolerance)
PicardIteration.mirror_property('mix_parameter')
PicardIteration.mirror_property('max_iterations')
PicardIteration.mirror_property('tolerance')
