import abc
from collections.abc import (Mapping,MutableMapping,Sequence)

from . import _flyft

__version__ = '0.0.0'

class MirrorClass(abc.ABCMeta):
    def __new__(mcls, name, bases, dct, *, mirrorclass=None):
        if mirrorclass is not None:
            dct['_mirrorclass'] = mirrorclass
        return super().__new__(mcls, name, bases, dct)

class Mirror(metaclass=MirrorClass):
    def __init__(self, *args, **kwargs):
        if len(args) == 1 and isinstance(args[0], self._mirrorclass):
            self._self = args[0]
        else:
            self._self = self._mirrorclass(*args,**kwargs)

    @classmethod
    def wrap(cls, obj):
        if not isinstance(obj, self._mirrorclass):
            raise TypeError('Cannot initialize from object')
        return cls(obj)

    @classmethod
    def mirror(cls, name, doc=None):
        attr = getattr(cls._mirrorclass, name, None)
        if attr is None:
            raise AttributeError('{} does not have attribute {}'.format(cls._mirrorclass.__name__,name))

        if doc is None:
            doc = attr.__doc__

        if isinstance(attr,property):
            fget = lambda self : getattr(self._self, name)
            if attr.fset is not None:
                fset = lambda self,v : setattr(self._self, name, v)
            else:
                fset = None
            mattr = property(fget=fget, fset=fset, doc=doc)
        else:
            mattr = lambda self,*args,**kwargs : attr(self._self,*args,**kwargs)

        setattr(cls, name, mattr)

class MirrorDict(Mapping):
    def __init__(self, _self):
        self._self = _self

    def __getitem__(self, key):
        return self._self[key]

    def __iter__(self):
        return self._self

    def __len__(self):
        return len(self._self)

class MirrorMutableDict(MutableMapping):
    def __init__(self, _self):
        self._self = _self

    def __delitem__(self, key):
        del self._self[key]

    def __getitem__(self, key):
        return self._self[key]

    def __iter__(self):
        return self._self

    def __setitem__(self, key, value):
        self._self[key] = value

    def __len__(self):
        return len(self._self)

class Mesh(Mirror,mirrorclass=_flyft.Mesh):
    def __init__(self, L, shape):
        super().__init__(L, shape)

    @property
    def coordinates(self):
        if not hasattr(self, '_coordinates'):
            self._coordinates = np.array([self._self.coordinate(i) for i in range(self.shape)])
        return self._coordinates
Mesh.mirror("L")
Mesh.mirror("shape")
Mesh.mirror("step")

class Field(Mirror,mirrorclass=_flyft.Field):
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
Field.mirror("shape")

class State(Mirror,mirrorclass=_flyft.State):
    def __init__(self, mesh, types):
        super().__init__(mesh, types)

    @property
    def fields(self):
        if not hasattr(self, '_fields'):
            self._fields = Fields(self._self)
        return self._fields

    @property
    def diameters(self):
        if not hasattr(self, '_diameters'):
            self._diameters = MirrorDict(self._self.diameters)
        return self._diameters

    @diameters.setter
    def diameters(self, value):
        self._self.diameters = value
        self._diameters = MirrorDict(self._self.diameters)

    @property
    def ideal_volumes(self):
        if not hasattr(self, '_ideal_volumes'):
            self._ideal_volumes = MirrorDict(self._self.ideal_volumes)
        return self._ideal_volumes

    @ideal_volumes.setter
    def ideal_volumes(self, value):
        self._self.ideal_volumes = value
        self._ideal_volumes = MirrorDict(self._self.ideal_volumes)

State.mirror("mesh")
