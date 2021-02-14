import abc
from collections.abc import Sequence

import _flyft

__version__ = '0.0.0'

class MirrorClass(metaclass=abc.ABCMeta):
    def __init__(self, *args, **kwargs):
        if len(args) == 1 and isinstance(args[0], self._class):
            self._self = args[0]
        else:
            self._self = self._class(*args,**kwargs)

    @classmethod
    def wrap(cls, obj):
        if not isinstance(obj, self._class):
            raise TypeError('Cannot initialize from object')
        return cls(obj)

    @classmethod
    def def_property(cls, name, settable=False, doc=None):
        if not hasattr(cls._class, name):
            raise AttributeError('Mirrored object does not have attribute')

        fget = lambda self : getattr(self._self, name)
        if settable:
            fset = lambda self,v : setattr(self._self, name, v)
        else:
            fset = None

        if doc is None and isinstance(cls[name],property):
            doc = cls[name].__doc__

        setattr(cls, name, property(fget, fset, doc))

class Mesh(MirrorClass):
    _class = _flyft.Mesh

    def __init__(self, L, shape):
        super().__init__(L, shape)

    @property
    def coordinates(self):
        if not hasattr(self, '_coordinates'):
            self._coordinates = np.array([self._self.coordinate(i) for i in range(self.shape)])
        return self._coordinates
Mesh.def_property("L")
Mesh.def_property("shape")
Mesh.def_property("step")

class Field(MirrorClass):
    _class = _flyft.Field

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
        return d
Field.def_property("shape")

class State(MirrorClass):
    _class = _flyft.State

    def __init__(self, mesh):
        super().__init__(mesh)

    class Fields(Sequence):
        def __init__(self, _self):
            self._self = _self

        def __getitem__(self, key):
            if not isinstance(key, int):
                raise TypeError('Fields are indexed by integer')

            if key < 0 or key >= self._self.num_fields:
                raise IndexError('Invalid field index')

            return Field.wrap(self._self.field[key])

        def __len__(self):
            return self._self.num_fields

    @property
    def fields(self):
        if not hasattr(self, '_fields'):
            self._fields = Fields(self._self)
        return self._fields

State.def_property("diameters", settable=True)
State.def_property("ideal_volumes", settable=True)
State.def_property("mesh")
