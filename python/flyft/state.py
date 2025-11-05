import numpy as np

from . import _flyft, mirror


class Communicator(mirror.Mirror, mirrorclass=_flyft.Communicator):
    r"""MPI communicator for parallel simulations.

    `Communicator` manages parallel communication between processes in
    distributed simulations. It provides information about the parallel
    environment and handles synchronization of data across processes.

    Attributes
    ----------
    size : int
        Total number of processes in the communicator.
    rank : int
        Rank (ID) of the current process.
    root : int
        Rank of the root process (typically 0).
    """

    size = mirror.Property()
    rank = mirror.Property()
    root = mirror.Property()


class Field(mirror.Mirror, mirrorclass=_flyft.Field):
    r"""Density field on a computational mesh.

    `Field` represents a density field discretized on a computational mesh.
    It provides access to density values.

    Parameters
    ----------
    shape : int
        Number of mesh points in the field.

    Attributes
    ----------
    shape : int
        Number of mesh points.
    data : numpy.ndarray
        Array view of the field data.

    Example
    -------
    Create a field and manipulate its data:

    .. code-block:: python

        field = flyft.Field(100)
        field.data = np.ones(100) * 0.5  # Set uniform density
        field[50] = 1.0  # Set specific point

    """

    def __init__(self, shape):
        super().__init__(shape)

    def __getitem__(self, key):
        return self.data[key]

    def __setitem__(self, key, value):
        self.data[key] = value

    def __array__(self):
        return self.data

    @property
    def data(self):
        # TODO: expose this using a cachable view property instead?
        return np.array(self._self, copy=False)

    @data.setter
    def data(self, value):
        v = np.array(
            value,
            dtype=np.float64,
            ndmin=1,
            copy=None if np.lib.NumpyVersion(np.__version__) >= "2.0.0" else False,
        )
        if v.shape != (self.shape,):
            raise TypeError("Array shapes must match")
        np.copyto(self.data, v)

    shape = mirror.Property()


class Fields(mirror.Mapping):
    r"""Collection of density fields for different particle types.

    `Fields` object provides dictionary-like access to density fields
    for different particle types. It automatically manages the mapping
    between particle type names and their corresponding `Field` objects.

    This class handles caching and ensures that field objects remain
    consistent with their underlying C++ counterparts.

    Example
    -------
    Access fields for different particle types:

    .. code-block:: python

        # Accessed through State.fields
        state.fields['A']  # Get field for particle type 'A'
        state.fields['B']  # Get field for particle type 'B'

    """

    def __init__(self, _self):
        self._self = _self
        self._cache = {}

    def __getitem__(self, key):
        if key not in self._cache or self._cache[key]._self is not self._self[key]:
            # using cache key in second condition is safe because it will only
            # be evaluated if key is in _cache
            self._cache[key] = Field.wrap(self._self[key])
        return self._cache[key]


class Mesh(mirror.Mirror, mirrorclass=_flyft.Mesh):
    r"""Base class for computational meshes.

    `Mesh` discretizes the domain for the density field. It sets up and provides information regarding
    mesh points, mesh volumes, and geometry boundaries.

    Attributes
    ----------
    L : float
        Domain length.
    shape : int
        Number of mesh points.
    step : float
        Mesh spacing.
    centers : numpy.ndarray
        Array of mesh point center positions.
    lower_boundary_condition : str
        Type of boundary condition at the lower boundary.
    upper_boundary_condition : str
        Type of boundary condition at the upper boundary.
    """

    @property
    def centers(self):
        if not hasattr(self, "_centers"):
            self._centers = np.array([self._self.center(i) for i in range(self.shape)])
        return self._centers

    def volume(self, bin=None):
        """Get volume of mesh element(s).

        Parameters
        ----------
        bin : int, optional
            Specific bin index. If None, returns total volume.

        Returns
        -------
        float
            Volume of the specified bin or total volume.
        """
        if bin is None:
            v = self._self.volume
        else:
            v = self._self.bin_volume(bin)
        return v

    def lower_bound(self, bin=None):
        """Get lower bound of mesh element(s).

        Parameters
        ----------
        bin : int, optional
            Specific bin index. If None, returns domain lower bound.

        Returns
        -------
        float
            Lower bound of the specified bin or domain.
        """
        if bin is None:
            lo = self._self.lower_bound
        else:
            lo = self._self.bin_lower_bound(bin)
        return lo

    def upper_bound(self, bin=None):
        """Get upper bound of mesh element(s).

        Parameters
        ----------
        bin : int, optional
            Specific bin index. If None, returns domain upper bound.

        Returns
        -------
        float
            Upper bound of the specified bin or domain.
        """
        if bin is None:
            u = self._self.upper_bound
        else:
            u = self._self.bin_upper_bound(bin)
        return u

    @staticmethod
    def _parse_boundary_condition(bc):
        if isinstance(bc, str):
            try:
                return getattr(_flyft.BoundaryType, bc)
            except AttributeError:
                raise ValueError("Unrecognized boundary type")
        elif isinstance(bc, _flyft.BoundaryType):
            return _flyft.BoundaryType(bc).name
        else:
            raise TypeError("Unrecognized boundary condition type")

    @property
    def lower_boundary_condition(self):
        return self._parse_boundary_condition(self._self.lower_boundary_condition)

    @property
    def upper_boundary_condition(self):
        return self._parse_boundary_condition(self._self.upper_boundary_condition)

    L = mirror.Property()
    shape = mirror.Property()
    step = mirror.Property()


class CartesianMesh(Mesh, mirrorclass=_flyft.CartesianMesh):
    r"""Cartesian mesh for one-dimensional systems.

    `CartesianMesh` provides a uniform discretization of a one-dimensional
    Cartesian domain.

    Parameters
    ----------
    L : float
        Domain length.
    shape : int
        Number of mesh points.
    boundary_condition : str or tuple
        Boundary condition type(s). Can be a single string for both boundaries
        or a tuple of (lower, upper) boundary conditions.
    area : float, optional
        Cross-sectional area for quasi-1D systems. Default is 1.0.

    Example
    -------
    Create a Cartesian mesh with periodic boundaries:

    .. code-block:: python

        mesh = flyft.CartesianMesh(L=10.0, shape=100, boundary_condition="periodic")

    Create a mesh with different boundary conditions:

    .. code-block:: python

        mesh = flyft.CartesianMesh(L=10.0, shape=100,
                                   boundary_condition=("reflect", "periodic"))

    """

    def __init__(self, L, shape, boundary_condition, area=1.0):
        if isinstance(boundary_condition, str):
            lower_bc = upper_bc = Mesh._parse_boundary_condition(boundary_condition)
        elif len(boundary_condition) == 2:
            lower_bc = Mesh._parse_boundary_condition(boundary_condition[0])
            upper_bc = Mesh._parse_boundary_condition(boundary_condition[1])
        super().__init__(0, L, shape, lower_bc, upper_bc, area)


class SphericalMesh(Mesh, mirrorclass=_flyft.SphericalMesh):
    r"""Spherical mesh for radially symmetric systems.

    `SphericalMesh` provides a uniform discretization of a spherical domain
    in radial coordinates. The mesh extends from r=0 to r=R with appropriate
    boundary conditions for spherical symmetry.

    Parameters
    ----------
    R : float
        Maximum radius of the spherical domain.
    shape : int
        Number of radial mesh points.
    area : float, optional
        Surface area scaling factor. Default is 4π for a full sphere.

    Notes
    -----
    The spherical mesh automatically applies Neumann boundary conditions at
    r=0 (center) and Dirichlet boundary conditions at r=R (surface) to
    ensure proper spherical symmetry.

    Example
    -------
    Create a spherical mesh for a droplet:

    .. code-block:: python

        mesh = flyft.SphericalMesh(R=5.0, shape=100)

    Create a spherical mesh with custom surface area:

    .. code-block:: python

        mesh = flyft.SphericalMesh(R=5.0, shape=100, area=2*math.pi)  # hemisphere

    """

    def __init__(self, R, shape, boundary_condition):
        upper_bc = Mesh._parse_boundary_condition(boundary_condition)
        super().__init__(0, R, shape, _flyft.BoundaryType.reflect, upper_bc)


class ParallelMesh(mirror.Mirror, mirrorclass=_flyft.ParallelMesh):
    """Parallel mesh wrapper for distributed computing.

    `ParallelMesh` wraps an existing mesh to enable distributed computing
    across multiple MPI processes. It provides access to both the full mesh
    and local mesh partitions for parallel computations.

    Parameters
    ----------
    mesh : Mesh
        The underlying mesh to parallelize.

    Attributes
    ----------
    full : Mesh
        The complete mesh across all processes.
    local : Mesh
        The local portion of the mesh for this process.

    Example
    -------
    Create a parallel wrapper for a Cartesian mesh:

    .. code-block:: python

        mesh = flyft.CartesianMesh(L=10.0, shape=100, boundary_condition="periodic")
        parallel_mesh = flyft.ParallelMesh(mesh)

        # Access full and local meshes
        full_mesh = parallel_mesh.full
        local_mesh = parallel_mesh.local

    """

    def __init__(self, mesh):
        communicator = Communicator()
        super().__init__(mesh, communicator)
        self._communicator = communicator

    full = mirror.Property()
    local = mirror.Property()


class State(mirror.Mirror, mirrorclass=_flyft.State):
    """System state for density functional theory simulations.

    `State` represents the complete state of a DFT system, including
    the computational mesh, field variables, and simulation time. It
    manages the density fields for all particle types in the system.

    Parameters
    ----------
    mesh : Mesh
        Computational mesh for the simulation domain.
    types : str or tuple of str
        Particle type names. Can be a single string or tuple of strings.

    Attributes
    ----------
    communicator : Communicator
        MPI communicator for parallel operations.
    mesh : Mesh
        The computational mesh.
    fields : Fields
        Collection of density fields for all particle types.
    time : float
        Current simulation time.

    Example
    -------
    Create a state for a single-component system:

    .. code-block:: python

        mesh = flyft.CartesianMesh(L=10.0, shape=100, boundary_condition="periodic")
        state = flyft.State(mesh, "A")

    Create a state for a binary mixture:

    .. code-block:: python

        state = flyft.State(mesh, ("A", "B"))

    Access density fields:

    .. code-block:: python

        rho_A = state.fields["A"]
        rho_A[:] = 0.5  # Set uniform density

    """

    def __init__(self, mesh, types):
        if isinstance(types, str):
            types = (types,)
        super().__init__(mesh, _flyft.VectorString(types))

    communicator = mirror.Property()
    mesh = mirror.Property()
    fields = mirror.WrappedProperty(Fields)

    time = mirror.Property()

    def gather_field(self, type_, rank=None):
        if rank is None:
            rank = self.communicator.root
        f = self._self.gather_field(type_, rank)

        if self.communicator.rank == rank:
            f = Field.wrap(f)
        else:
            f = None

        return f
