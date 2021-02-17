import abc
import collections.abc

class MirrorMeta(abc.ABCMeta):
    _register = {}

    def __new__(mcls, name, bases, dct, *, mirrorclass=None):
        if mirrorclass is not None:
            dct['_mirrorclass'] = mirrorclass
        cls = super().__new__(mcls, name, bases, dct)

        if mirrorclass is not None:
            if mirrorclass in mcls._register:
                raise TypeError('Type has already been mirrored')
            mcls._register[mirrorclass] = cls

        return cls

class Mirror(metaclass=MirrorMeta):
    def __init__(self, *args, **kwargs):
        args_ = []
        for a in args:
            if isinstance(a,Mirror):
                args_.append(a._self)
            else:
                args_.append(a)

        kwargs_ = {}
        for key,value in kwargs.items():
            if isinstance(value,Mirror):
                kwargs_[key] = value._self
            else:
                kwargs_[key] = value

        self._self = self._mirrorclass(*args_,**kwargs_)

    @classmethod
    def wrap(cls, ptr):
        if not isinstance(ptr, cls._mirrorclass):
            raise TypeError('Cannot wrap object type')
        # bypass calling this class's __init__, and just force in the _self value
        obj = cls.__new__(cls)
        super(Mirror,obj).__init__()
        obj._self = ptr
        return obj

    @classmethod
    def mirror(cls, name, doc=None):
        attr = getattr(cls._mirrorclass, name, None)
        if attr is None:
            raise AttributeError('{} does not have attribute {}'.format(cls._mirrorclass.__name__,name))

        if doc is None:
            doc = attr.__doc__

        if isinstance(attr,property):
            def fget(obj):
                # get value from the mirror class
                v = getattr(obj._self, name, None)
                if v is None:
                    raise AttributeError('Mirror class does not have attribute {}'.format(name))

                # check for mirror class that *may* be cached as obj._name
                cache_name = '_'+name
                cache_v = getattr(obj, cache_name, None)
                update_cache = False

                # if cache value is set, check whether it is still current
                if cache_v is not None:
                    if isinstance(cache_v,Mirror):
                        # if mirror internal object still matches pointer, reuse the cache
                        if cache_v._self is v:
                            v = cache_v
                        # otherwise, create a new wrap and mark to update the cache
                        else:
                            v = MirrorMeta._register[type(v)].wrap(v)
                            update_cache = True
                    else:
                        raise TypeError('Mirrored properties must only cache mirror type')
                # otherwise, use the output directly, wrapping a class if it is known to be mirrored
                else:
                    type_ = type(v)
                    # autowrap the mirrorclass
                    if type_ in MirrorMeta._register:
                        v = MirrorMeta._register[type_].wrap(v)
                        update_cache = True

                # if object should be cached, set the attribute before finalizing
                if update_cache:
                    setattr(obj,cache_name,v)

                return v

            def fset(obj,v):
                # autoconvert mirrorclasses
                if isinstance(v, Mirror):
                    # save instance to cache
                    setattr(obj, '_'+name, v)
                    setattr(obj._self, name, v._self)
                else:
                    setattr(obj._self, name, v)

            mattr = property(fget=fget,
                             fset=fset if attr.fset is not None else None,
                             doc=doc)
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
