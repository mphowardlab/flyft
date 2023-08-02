import numpy as np

from . import _flyft
from . import mirror

class Communicator(mirror.Mirror,mirrorclass=_flyft.Communicator):
    size = mirror.Property()
    rank = mirror.Property()
    root = mirror.Property()

class Field(mirror.Mirror,mirrorclass=_flyft.Field):
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
        return np.array(self._self,copy=False)

    @data.setter
    def data(self, value):
        v = np.array(value, dtype=np.float64, ndmin=1, copy=False)
        if v.shape != (self.shape,):
            raise TypeError('Array shapes must match')
        np.copyto(self.data, v)

    shape = mirror.Property()

class Fields(mirror.Mapping):
    def __init__(self, _self):
        self._self = _self
        self._cache = {}

    def __getitem__(self, key):
        if key not in self._cache or self._cache[key]._self is not self._self[key]:
            # using cache key in second condition is safe because it will only be evaluated if key is in _cache
             self._cache[key] = Field.wrap(self._self[key])
        return self._cache[key]

class Mesh(mirror.Mirror,mirrorclass=_flyft.Mesh):
    @property
    def centers(self):
        if not hasattr(self, '_centers'):
            self._centers = np.array([self._self.center(i) for i in range(self.shape)])
        return self._centers

    def volume(self, bin = None):
        if bin is None:
            v = self._self.volume
        else:
            v = self._self.bin_volume(bin)
        return v
         
    def lower_bound(self, bin=None):
        if bin is None:
            l = self._self.lower_bound
        else:
            l = self._self.bin_lower_bound(bin)
        return l
    
    def upper_bound(self, bin=None):
        if bin is None:
            u = self._self.upper_bound
        else:
            u = self._self.bin_upper_bound(bin)
        return u
    
    @staticmethod
    def _parse_boundary_condition(bc):
        if isinstance(bc, str):
            lower_boundary_condition = upper_boundary_condition = getattr(_flyft.BoundaryType, bc, None)
        elif len(bc) == 2:
            lower_boundary_condition = getattr(_flyft.BoundaryType, bc[0], None)
            upper_boundary_condition = getattr(_flyft.BoundaryType, bc[1], None)
        else:
            raise TypeError("Boundary type must be str or 2-tuple")

        if lower_boundary_condition is None or upper_boundary_condition is None:
            raise ValueError("Unrecognized boundary type")
            
        return lower_boundary_condition,upper_boundary_condition
        
    
    boundary_type = _flyft.BoundaryType
    
    lower_boundary_condition = mirror.Property()
    upper_boundary_condition = mirror.Property()
    L = mirror.Property()
    shape = mirror.Property()
    step = mirror.Property()

class CartesianMesh(Mesh,mirrorclass=_flyft.CartesianMesh):
    def __init__(self,lower,upper,shape, boundary_condition, area = 1.): 
        lower_boundary_condition,upper_boundary_condition = Mesh._parse_boundary_condition(boundary_condition)
        super().__init__(lower,upper,shape, lower_boundary_condition,upper_boundary_condition,area)
        
        
class SphericalMesh(Mesh,mirrorclass=_flyft.SphericalMesh):
    def __init__(self,lower,upper,shape,boundary_condition):
        lower_boundary_condition,upper_boundary_condition = Mesh._parse_boundary_condition(boundary_condition)
        super().__init__(lower,upper,shape,Mesh.boundary_type.reflect,upper_boundary_condition)
        

class ParallelMesh(mirror.Mirror,mirrorclass=_flyft.ParallelMesh):
    def __init__(self,mesh):
        communicator = Communicator()
        super().__init__(mesh,communicator)
        self._communicator = communicator

    full = mirror.Property()
    local = mirror.Property()

class State(mirror.Mirror,mirrorclass=_flyft.State):
    def __init__(self, mesh, types): 
        if isinstance(types, str):
            types = (types,)
        super().__init__(mesh,_flyft.VectorString(types))  
        
    communicator = mirror.Property()
    mesh = mirror.Property()
    fields = mirror.WrappedProperty(Fields)
 
    time = mirror.Property()

    def gather_field(self, type_, rank=None):
        if rank is None:
            rank = self.communicator.root
        f = self._self.gather_field(type_,rank)

        if self.communicator.rank == rank:
            f = Field.wrap(f)
        else:
            f = None

        return f
