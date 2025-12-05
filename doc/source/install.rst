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

Then, make a `build` directory and change directory to it

.. code:: bash

    mkdir build
    cd build

Create a virtual environment in the `build` directory and activate it

.. code:: bash

    conda create -p ./env python=3.11
    conda activate ./env

Now, install the packages required for building ``flyft`` and can be installed using the 
requirements file

.. code:: bash

    pip install -r requirements.txt

Finally, to install ``flyft``, run the following command from the `build` directory

.. code:: bash

    cmake ..
    make
    make install

.. note::

    Before running `make install`, make sure the `CMAKE_INSTALL_PREFIX` environment variable 
    points to your conda environment. Check it by going to the build directory 
    and run the following command: 
    
    .. code:: bash

        ccmake ..
    
    `CMAKE_INSTALL_PREFIX` variable should have the path of the site packages directory of your conda
    environment. 
    
    You can find the path of the site packages directory by running the following command 
    
    .. code:: bash

        python -c "import site; print(site.getsitepackages()[0])"

A suite of unit tests is provided with the source code and can be run with `pytest` from the `build` directory:

.. code:: bash

    pytest python/

You can build the documentation from source with:

.. code:: bash

    cd doc
    pip install -r doc/requirements.txt 
    make html
