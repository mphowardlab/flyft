from . import _flyft, mirror


class Solver(mirror.Mirror, mirrorclass=_flyft.Solver):
    solve = mirror.Method()


class PicardIteration(Solver, mirrorclass=_flyft.PicardIteration):
    def __init__(self, mix_parameter, max_iterations, tolerance):
        super().__init__(mix_parameter, max_iterations, tolerance)

    mix_parameter = mirror.Property()
    max_iterations = mirror.Property()
    tolerance = mirror.Property()
