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
            if isinstance(a, Mirror):
                args_.append(a._self)
            else:
                args_.append(a)

        kwargs_ = {}
        for key,value in kwargs.items():
            if isinstance(value, Mirror):
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

class Method:
    def __init__(self, doc=None):
        self.__doc__ = doc

    def __set_name__(self, owner, name):
        self.name = name

    def __get__(self, obj, objtype=None):
        if obj is None:
            return self

        try:
            meth = getattr(obj._mirrorclass, self.name)
        except:
            raise AttributeError('{} does not have method {}'.format(obj._mirrorclass.__name__,self.name))

        def mmeth(*args, **kwargs):
            args_ = []
            for a in args:
                if isinstance(a, Mirror):
                    args_.append(a._self)
                else:
                    args_.append(a)

            kwargs_ = {}
            for key,value in kwargs.items():
                if isinstance(value, Mirror):
                    kwargs_[key] = value._self
                else:
                    kwargs_[key] = value

            return meth(obj._self,*args_,**kwargs_)

        return mmeth

class Property:
    def __init__(self, doc=None):
        self.__doc__ = doc

    def __set_name__(self, owner, name):
        self.name = name
        self.cache_name = '_' + name

    def __get__(self, obj, objtype=None):
        if obj is None:
            return self

        # get value from the mirror class
        try:
            v = getattr(obj._self, self.name)
        except:
            raise AttributeError('Mirror class does not have attribute {}'.format(self.name))

        # check for mirror class that *may* be cached
        has_cache = hasattr(obj, self.cache_name)
        cache_v = getattr(obj, self.cache_name, None)
        update_cache = False

        # if cache value is set, check whether it is still current
        if has_cache:
            if isinstance(cache_v, Mirror):
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
            setattr(obj, self.cache_name, v)

        return v

    def __set__(self, obj, value):
        try:
            attr = getattr(obj._mirrorclass, self.name)
            if not isinstance(attr,property):
                raise AttributeError
        except AttributeError:
            raise AttributeError('{} does not have property {}'.format(obj._mirrorclass.__name__,self.name))
        if attr.fset is None:
            raise AttributeError('{} property {} is read only'.format(obj._mirrorclass.__name__,self.name))

        if isinstance(value, Mirror):
            # cache Mirror objects
            setattr(obj, self.cache_name, value)
            setattr(obj._self, self.name, value._self)
        else:
            # don't cache non-Mirror objects
            if hasattr(obj, self.cache_name):
                delattr(obj, self.cache_name)
            setattr(obj._self, self.name, value)


class WrappedProperty:
    def __init__(self, wrapper, doc=None):
        self.wrapper = wrapper
        self.__doc__ = doc

    def __set_name__(self, owner, name):
        self.name = name
        self.cache_name = '_' + name

    def __get__(self, obj, objtype=None):
        if obj is None:
            return self

        # get value from the mirror class
        try:
            v = getattr(obj._self, self.name)
        except:
            raise AttributeError('Mirror class does not have attribute {}'.format(name))

        # check for mirror mapping that *may* be cached as obj._name
        has_cache = hasattr(obj, self.cache_name)
        cache_v = getattr(obj, self.cache_name, None)

        if not has_cache or cache_v._self is not v:
            cache_v = self.wrapper(v)
            setattr(obj, self.cache_name, cache_v)
        return cache_v

    def __set__(self, obj, value):
        try:
            attr = getattr(obj._mirrorclass, self.name)
            if not isinstance(attr,property):
                raise AttributeError
        except AttributeError:
            raise AttributeError('{} does not have property {}'.format(obj._mirrorclass.__name__,self.name))
        if attr.fset is None:
            raise AttributeError('{} property {} is read only'.format(obj._mirrorclass.__name__,self.name))

        if isinstance(value, self.wrapper):
            # use value directly
            setattr(obj, self.cache_name, value)
            setattr(obj._self, self.name, value._self)
        elif issubclass(self.wrapper, collections.abc.MutableMapping):
            # create new mapping
            container = type(getattr(obj._self, self.name))
            mirror_v = container()
            for t,v in value.items():
                mirror_v[t] = v
            setattr(obj._self, self.name, mirror_v)
        elif issubclass(self.wrapper, collections.abc.MutableSequence):
            # create new sequence
            container = type(getattr(obj._self, self.name))
            mirror_v = container()
            for v in value:
                mirror_v.append(v)
            setattr(obj._self, self.name, mirror_v)
        else:
            raise TypeError('Cannot convert value to appropriate wrapped type')

class Sequence(collections.abc.Sequence):
    def __init__(self, _self):
        self._self = _self

    def __getitem__(self, key):
        return self._self[key]

    def __len__(self):
        return len(self._self)

class MutableSequence(collections.abc.MutableSequence):
    def __init__(self, _self):
        self._self = _self

    def __delitem__(self, key):
        del self._self[key]

    def __getitem__(self, key):
        return self._self[key]

    def __setitem__(self, key, value):
        self._self[key] = value

    def __len__(self):
        return len(self._self)

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
