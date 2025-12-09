============
Installation
============

Building from source
====================

``flyft`` currently can only be installed from source.

Before installing ``flyft``, the pre-requisite software should be installed.

    * `CMake (version > 3.18) <https://cmake.org/cmake/help/latest/#>`__
    * `FFTW <https://www.fftw.org/>`__


First, clone the repository from GitHub:

.. code:: bash

    git clone git@github.com:mphowardlab/flyft.git
    cd flyft

Then, make a `build` directory with a conda environment using the following command:

.. code:: bash

    conda env create --prefix=build/env -f env.yml

After running the aforementioned command, activate the conda environment using:

.. code:: bash

    conda activate build/env


Configure ``flyft`` with CMake:

.. code:: bash

    cmake -B ./build

Build ``flyft``:

.. code:: bash

    cmake --build build

Install ``flyft``:

.. code:: bash

    cmake --install build

A suite of unit tests is provided with the source code and can be run with `pytest` from the `build` directory:

.. code:: bash

    pytest python/

You can build the documentation from source with:

.. code:: bash

    cd doc
    pip install -r requirements.txt
    make html
