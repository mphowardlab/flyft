import flyft.mirror
import pytest


class _A:
    def __init__(self, x, y):
        self._x = x
        self._y = y

    @property
    def x(self):
        return self._x

    @x.setter
    def x(self, value):
        self._x = value

    @property
    def y(self):
        return self._y

    def add(self, z):
        return self.x + self.y + z


class A(flyft.mirror.Mirror, mirrorclass=_A):
    def __init__(self, x, y):
        super().__init__(x, y)

    x = flyft.mirror.Property()
    y = flyft.mirror.Property()

    add = flyft.mirror.Method()


def test_Mirror():
    a = A(1, 2)

    # test creation
    assert isinstance(a._self, _A)

    # test read properties
    assert a.x == 1
    assert a._self.x == 1
    assert a.y == 2
    assert a._self.y == 2

    # test setter
    a.x = -1
    assert a.x == -1
    assert a._self.x == -1
    assert a.y == 2
    assert a._self.y == 2

    # test method
    assert a.add(3) == 4


def test_Mirror_wrap():
    _a = _A(3, 4)
    a = A.wrap(_a)

    assert a._self is _a
    assert a.x == 3
    assert a.y == 4


def test_Mapping():
    d = dict(a=1, b=2)
    m = flyft.mirror.Mapping(d)

    # length
    assert len(m) == 2

    # get
    assert m["a"] == 1
    assert m["b"] == 2

    # iteration
    for key, value in m.items():
        assert d[key] == value

    # not mutable
    with pytest.raises(TypeError):
        m["a"] = 3


def test_MutableMapping():
    d = dict(a=1, b=2)
    m = flyft.mirror.MutableMapping(d)

    # length
    assert len(m) == 2

    # get
    assert m["a"] == 1
    assert m["b"] == 2

    # iteration
    for key, value in m.items():
        assert d[key] == value

    # set
    m["a"] = 3
    assert m["a"] == 3
    assert d["a"] == 3

    # delete
    del m["b"]
    assert "b" not in m
    assert "b" not in d
