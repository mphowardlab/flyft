cmake_minimum_required(VERSION 3.19)
project(
    flyft
    VERSION 0.0.0
    LANGUAGES CXX
    )

option(FLYFT_OPENMP "Use OpenMP threading." ON)
option(FLYFT_PYTHON "Build Python package." ON)
option(FLYFT_TESTING "Build testing." ON)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
find_package(FFTW3 MODULE REQUIRED)
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
endif()
if(FLYFT_TESTING)
    add_subdirectory(tests)
endif()
