import abc
from collections.abc import Sequence

import _flyft

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
State.mirror("diameters")
State.mirror("ideal_volumes")
State.mirror("mesh")
