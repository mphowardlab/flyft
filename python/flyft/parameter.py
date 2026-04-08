from . import _flyft, mirror


class CustomParameter(_flyft.DoubleParameter):
    """Custom parameter class for user-defined time-dependent behavior.

    A `CustomParameter` allows users to define arbitrary time-dependent
    parameter behavior by subclassing and implementing custom evaluation
    methods. This provides maximum flexibility for complex time protocols.

    Example
    -------
    Create a custom parameter with user-defined behavior:

    .. code-block:: python

        class MyParameter(flyft.CustomParameter):
            def __call__(self, state):
                # Custom time-dependent behavior
                return some_function(state.time)

    """

    pass


class _DoubleParameter(mirror.Mirror, mirrorclass=_flyft.DoubleParameter):
    """Base class for time-dependent double-valued parameters.

    A `_DoubleParameter` provides the base interface for parameters that
    return double values and can depend on the system state and time.
    All parameter classes implement the callable interface to evaluate
    their value at a given state.
    """

    __call__ = mirror.Method()


class LinearParameter(_DoubleParameter, mirrorclass=_flyft.LinearParameter):
    """Linear time-dependent parameter.

    Implements a parameter that varies linearly with time:

    .. math::
        p(t) = p_0 + r \\cdot (t - t_0)

    where :math:`p_0` is the initial value, :math:`r` is the rate of change,
    :math:`t` is the current time, and :math:`t_0` is the origin time.

    This is useful for implementing linear ramps, constant rates of change,
    or linear boundary condition variations.

    Parameters
    ----------
    initial : float
        Initial parameter value at the origin time.
    origin : float
        Reference time point.
    rate : float
        Rate of change of the parameter per unit time.

    Attributes
    ----------
    initial : float
        Initial parameter value.
    origin : float
        Reference time.
    rate : float
        Rate of change.

    Example
    -------
    Create a linear parameter that decreases at rate 0.1 per time unit:

    .. code-block:: python

        param = flyft.LinearParameter(initial=1.0, origin=0.0, rate=-0.1)

    """

    def __init__(self, initial, origin, rate):
        super().__init__(initial, origin, rate)

    initial = mirror.Property()
    origin = mirror.Property()
    rate = mirror.Property()


class SquareRootParameter(_DoubleParameter, mirrorclass=_flyft.SquareRootParameter):
    """Square root time-dependent parameter.

    Implements a parameter that varies as the square root of time:

    .. math::
        p(t) = \\sqrt{p_0^2 + r \\cdot (t - t_0)}

    where :math:`p_0` is the initial value, :math:`r` is the rate coefficient,
    :math:`t` is the current time, and :math:`t_0` is the origin time.

    This functional form is useful for modeling parameters that evolve with a
    square root time dependence.

    Parameters
    ----------
    initial : float
        Initial parameter value at the origin time.
    origin : float
        Reference time point.
    rate : float
        Rate coefficient for the square root dependence.

    Attributes
    ----------
    initial : float
        Initial parameter value.
    origin : float
        Reference time.
    rate : float
        Rate coefficient.

    Example
    -------
    Create a square root parameter:

    .. code-block:: python

        param = flyft.SquareRootParameter(initial=1.0, origin=0.0, rate=-0.5)

    """

    def __init__(self, initial, origin, rate):
        super().__init__(initial, origin, rate)

    initial = mirror.Property()
    origin = mirror.Property()
    rate = mirror.Property()
