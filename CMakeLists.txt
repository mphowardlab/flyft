cmake_minimum_required(VERSION 3.19)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(
    flyft
    VERSION 0.0
    LANGUAGES CXX
    )

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(FFTW3 MODULE)

add_subdirectory(src)
add_subdirectory(tests)
