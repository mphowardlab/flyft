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
                print(mcls._register)
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

    @classmethod
    def mirror_method(cls, name, doc=None):
        try:
            meth = getattr(cls._mirrorclass, name)
        except AttributeError:
            raise AttributeError('{} does not have method {}'.format(cls._mirrorclass.__name__,name))

        if doc is None:
            doc = meth.__doc__

        def mmeth(obj, *args, **kwargs):
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
        setattr(cls, name, mmeth)

    @classmethod
    def mirror_property(cls, name, doc=None):
        try:
            attr = getattr(cls._mirrorclass, name)
            if not isinstance(attr,property):
                raise AttributeError
        except AttributeError:
            raise AttributeError('{} does not have property {}'.format(cls._mirrorclass.__name__,name))

        if doc is None:
            doc = attr.__doc__

        # name used to save mirror classes as obj._name
        cache_name = '_'+name

        def fget(obj):
            # get value from the mirror class
            try:
                v = getattr(obj._self, name)
            except:
                raise AttributeError('Mirror class does not have attribute {}'.format(name))

            # check for mirror class that *may* be cached
            has_cache = hasattr(obj, cache_name)
            cache_v = getattr(obj, cache_name, None)
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
                setattr(obj, cache_name, v)

            return v

        def fset(obj,value):
            if isinstance(value, Mirror):
                # cache Mirror objects
                setattr(obj, cache_name, value)
                setattr(obj._self, name, value._self)
            else:
                # don't cache non-Mirror objects
                if hasattr(obj, cache_name):
                    delattr(obj, cache_name)
                setattr(obj._self, name, value)

        mattr = property(fget=fget,
                         fset=fset if attr.fset is not None else None,
                         doc=doc)
        setattr(cls, name, mattr)

    @classmethod
    def mirror_mapped_property(cls, name, mutable=True, doc=None):
        try:
            attr = getattr(cls._mirrorclass, name)
            if not isinstance(attr,property):
                raise AttributeError
        except AttributeError:
            raise AttributeError('{} does not have property {}'.format(cls._mirrorclass.__name__,name))

        if doc is None:
            doc = attr.__doc__

        def fget(obj):
            # get value from the mirror class
            try:
                v = getattr(obj._self, name, None)
            except:
                raise AttributeError('Mirror class does not have attribute {}'.format(name))

            # check for mirror mapping that *may* be cached as obj._name
            cache_name = '_'+name
            has_cache = hasattr(obj, cache_name)
            cache_v = getattr(obj, cache_name,None)

            if not has_cache or cache_v._self is not v:
                if mutable:
                    cache_v = MutableMapping(v)
                else:
                    cache_v = Mapping(v)
                setattr(obj, cache_name, cache_v)
            return cache_v

        def fset(obj,value):
            # deduce type of internal container, then make a new one
            container = type(getattr(obj._self, name))
            mirror_v = container()
            for t,v in value.items():
                mirror_v[t] = v
            setattr(obj._self, name, mirror_v)

        mattr = property(fget=fget,
                         fset=fset if attr.fset is not None else None,
                         doc=doc)
        setattr(cls, name, mattr)

class CompositeMixin:
    @property
    def objects(self):
        if not hasattr(self,'_objects'):
            self._objects = []
        return self._objects

    @objects.setter
    def objects(self, objects):
        self._self.clear()
        self._objects = []
        try:
            objects = list(objects)
        except TypeError:
            objects = [objects]
        self.extend(objects)

    def __getitem__(self, key):
        return self.objects[key]

    def __iter__(self):
        return iter(self.objects)

    def __len__(self):
        return len(self.objects)

    def append(self, object_):
        if object_ not in self.objects:
            self._self.append(object_._self)
            self.objects.append(object_)

    def remove(self, object_):
        self._self.remove(object_._self)
        self.objects.remove(object_)

    def extend(self, objects):
        for obj in objects:
            self.append(obj)

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
