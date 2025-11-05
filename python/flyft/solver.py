from . import _flyft, mirror


class Solver(mirror.Mirror, mirrorclass=_flyft.Solver):
    r"""Base class for iterative solvers in density functional theory.
    
    `Solver` implements iterative algorithms to find equilibrium density
    distributions by minimizing the grand potential functional. The solver
    iteratively updates density fields until convergence is achieved.
    
    All solver classes implement the `solve` method to perform the iterative
    procedure and find the equilibrium solution.
    """
    
    solve = mirror.Method()


class PicardIteration(Solver, mirrorclass=_flyft.PicardIteration):
    r"""Picard iteration solver for equilibrium DFT calculations.
    
    Implements Picard iteration method (also known as fixed-point iteration)
    to solve the equilibrium condition in density functional theory:
    
    .. math::
        \\rho_{i}^{j+1}(\\mathbf{r}) = (1-\\alpha) \\rho_i^{j}(\\mathbf{r}) + \\alpha \\rho_i^{j}(\\mathbf{r})

    where :math:`\\alpha` is the mixing parameter, :math:`\\rho_i^{j+1}(\\mathbf{r})` is the new density field 
    value, and :math:`\\rho_i^{j}(\\mathbf{r})` is the current estimate of the density field which is given by:
    
    .. math::
        \\rho_i^{j}(\\mathbf{r}) = \\exp\\left(\\beta \\frac{\\delta F_{ex}}{\\delta \\rho_i(\\mathbf{r})} - \\beta V_{ext,i}(\\mathbf{r})\\right)

    The mixing parameter controls convergence stability: smaller values provide
    better stability but slower convergence, while larger values converge
    faster but may become unstable.
    
    Parameters
    ----------
    mix_parameter : float
        Mixing parameter for controlling convergence (0 < α ≤ 1).
    max_iterations : int
        Maximum number of iterations before stopping.
    tolerance : float
        Convergence tolerance for the density change.
        
    Attributes
    ----------
    mix_parameter : float
        Mixing parameter value.
    max_iterations : int
        Maximum iterations allowed.
    tolerance : float
        Convergence tolerance.
        
    Example
    -------
    Create a Picard iteration solver:
    
    .. code-block:: python

        solver = flyft.solver.PicardIteration(
            mix_parameter=0.1,
            max_iterations=1000,
            tolerance=1e-8
        )
        
    """
    
    def __init__(self, mix_parameter, max_iterations, tolerance):
        super().__init__(mix_parameter, max_iterations, tolerance)

    mix_parameter = mirror.Property()
    max_iterations = mirror.Property()
    tolerance = mirror.Property()
