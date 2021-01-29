find_path(
    FFTW3_INCLUDE_DIR
    NAMES "fftw3.h"
    PATH_SUFFIXES "include"
    )
find_library(
    FFTW3_LIBRARY
    NAMES "fftw3"
    PATH_SUFFIXES "lib" "lib64"
    )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFTW3 REQUIRED_VARS FFTW3_INCLUDE_DIR FFTW3_LIBRARY)
mark_as_advanced(FFTW3_FOUND FFTW3_INCLUDE_DIR FFTW3_LIBRARY)

if(FFTW3_FOUND)
    if(NOT TARGET FFTW3::fftw3)
        add_library(FFTW3::fftw3 SHARED IMPORTED)
        set_target_properties(
            FFTW3::fftw3
            PROPERTIES
            IMPORTED_LOCATION ${FFTW3_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES ${FFTW3_INCLUDE_DIR}
        )
    endif()
endif()
