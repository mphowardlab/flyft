from . import mirror

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

class IterativeAlgorithmMixin:
    max_iterations = mirror.Property()
    tolerance = mirror.Property()

class FixedPointAlgorithmMixin(IterativeAlgorithmMixin):
    mix_parameter = mirror.Property()
