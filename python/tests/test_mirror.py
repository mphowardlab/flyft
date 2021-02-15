import numpy as np
import pytest

import flyft

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
        return self.x+self.y+z

class A(flyft.Mirror,mirrorclass=_A):
    def __init__(self, x, y):
        super().__init__(x,y)
A.mirror("x")
A.mirror("y")
A.mirror("add")

def test_mirror():
    a = A(1,2)

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
