import abc
import collections.abc

class MirrorMeta(abc.ABCMeta):
    def __new__(mcls, name, bases, dct, *, mirrorclass=None):
        if mirrorclass is not None:
            dct['_mirrorclass'] = mirrorclass
        return super().__new__(mcls, name, bases, dct)

class Mirror(metaclass=MirrorMeta):
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

class Mapping(collections.abc.Mapping):
    def __init__(self, _self):
        self._self = _self

    def __getitem__(self, key):
        return self._self[key]

    def __iter__(self):
        return iter(self._self)

    def __len__(self):
        return len(self._self)

class MutableMapping(collections.abc.MutableMapping):
    def __init__(self, _self):
        self._self = _self

    def __delitem__(self, key):
        del self._self[key]

    def __getitem__(self, key):
        return self._self[key]

    def __iter__(self):
        return iter(self._self)

    def __setitem__(self, key, value):
        self._self[key] = value

    def __len__(self):
        return len(self._self)
