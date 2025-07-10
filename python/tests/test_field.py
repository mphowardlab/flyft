import numpy as np
import pytest

import flyft


@pytest.fixture
def field():
    return flyft.Field(5)


def test_init(field):
    assert field.shape == 5
    assert np.allclose(field.data, np.zeros(5))


def test_accessors(field):
    # by index
    assert field[0] == pytest.approx(0)
    field[0] = 1
    assert field[0] == pytest.approx(1)

    # by slice
    assert np.allclose(field[:2], [1, 0])
    field[:2] = [1, 2]
    assert np.allclose(field[:2], [1, 2])

    # set all
    field[:] = [1, 2, 3, 4, 5]
    assert np.allclose(field.data, [1, 2, 3, 4, 5])

    # use infix op, which should trigger setter
    field.data *= 2.0
    assert np.allclose(field.data, [2, 4, 6, 8, 10])


def test_mirror(field):
    assert field._self.shape == 5

    field.data = [1, 2, 3, 4, 5]
    vals = [field._self[i] for i in range(field.shape)]
    assert np.allclose(vals, [1, 2, 3, 4, 5])
