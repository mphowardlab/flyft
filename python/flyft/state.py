import numpy as np

from . import _flyft
from . import mirror

class Field(mirror.Mirror,mirrorclass=_flyft.Field):
    def __init__(self, shape):
        super().__init__(shape)

    def __getitem__(self, key):
        return self.data[key]

    def __setitem__(self, key, value):
        self.data[key] = value

    @property
    def data(self):
        if not hasattr(self, '_data'):
            self._data = np.array(self._self,copy=False)
        return self._data

    @data.setter
    def data(self, value):
        v = np.array(value, dtype=np.float64, ndmin=1, copy=False)
        if v.shape != (self.shape,):
            raise TypeError('Array shapes must match')
        np.copyto(self.data, v)
Field.mirror('shape')

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
    def __init__(self, L, shape):
        super().__init__(L, shape)

    @property
    def coordinates(self):
        if not hasattr(self, '_coordinates'):
            self._coordinates = np.array([self._self.coordinate(i) for i in range(self.shape)])
        return self._coordinates
Mesh.mirror('L')
Mesh.mirror('shape')
Mesh.mirror('step')

class State(mirror.Mirror,mirrorclass=_flyft.State):
    def __init__(self, mesh, types):
        # cast type list into vector
        if isinstance(types, str):
            types = (types,)
        super().__init__(mesh,_flyft.VectorString(types))

        # manually store input mesh into the cache
        self._mesh = mesh

    @property
    def fields(self):
        if not hasattr(self, '_fields'):
            self._fields = Fields(self._self.fields)
        return self._fields

    @property
    def diameters(self):
        if not hasattr(self, '_diameters'):
            self._diameters = mirror.MutableMapping(self._self.diameters)
        return self._diameters

    @diameters.setter
    def diameters(self, value):
        diams = _flyft.TypeMapDouble()
        for t,d in value.items():
            diams[t] = d
        self._self.diameters = diams
        self._diameters = mirror.MutableMapping(self._self.diameters)

    @property
    def ideal_volumes(self):
        if not hasattr(self, '_ideal_volumes'):
            self._ideal_volumes = mirror.MutableMapping(self._self.ideal_volumes)
        return self._ideal_volumes

    @ideal_volumes.setter
    def ideal_volumes(self, value):
        vols = _flyft.TypeMapDouble()
        for t,v in value.items():
            vols[t] = v
        self._self.ideal_volumes = vols
        self._ideal_volumes = mirror.MutableMapping(self._self.ideal_volumes)
State.mirror('mesh')