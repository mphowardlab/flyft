cmake_minimum_required(VERSION 3.19)
project(
    flyft
    VERSION 0.0.0
    LANGUAGES CXX
    )

# set default build type https://blog.kitware.com/cmake-and-the-default-build-type/
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

option(FLYFT_MPI "Use MPI." ON)
option(FLYFT_OPENMP "Use OpenMP threading." OFF)
option(FLYFT_PYTHON "Build Python package." ON)
option(FLYFT_TESTING "Build testing." ON)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
find_package(FFTW3 MODULE REQUIRED)
if(FLYFT_MPI)
    find_package(MPI REQUIRED)
endif()
if(FLYFT_OPENMP)
    find_package(OpenMP REQUIRED)
    if(NOT TARGET FFTW3::fftw3_omp)
        message(FATAL_ERROR "Unable to locate FFTW3 OpenMP library")
    endif()
endif()

if(FLYFT_TESTING)
    enable_testing()
endif()

add_subdirectory(src)

if(FLYFT_PYTHON)
    add_subdirectory(python)
    # set(PYTHON_SITE_INSTALL_DIR "${_python_site_package_rel}/flyft" CACHE PATH "Python site-packages directory (relative to CMAKE_INSTALL_PREFIX)" FORCE)
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development)
execute_process(
    COMMAND ${Python_EXECUTABLE} -c "import site; print(site.getsitepackages()[0])"
    OUTPUT_VARIABLE _python_site_packages
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_VARIABLE _error
)

if(_error)
    message(FATAL_ERROR "Failed to find Python site-packages: ${_error}")
endif()

set(CMAKE_INSTALL_PREFIX ${_python_site_packages} CACHE PATH "Install prefix" FORCE)
message(STATUS "Setting CMAKE_INSTALL_PREFIX to Python site-packages: ${CMAKE_INSTALL_PREFIX}")
