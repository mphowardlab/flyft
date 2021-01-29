add_library(flyft SHARED
    field.cc
    fourier_transform.cc
    free_energy_functional.cc
    mesh.cc
    rosenfeld_fmt.cc
    state.cc
    )

target_include_directories(
    flyft
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

target_compile_features(flyft PUBLIC cxx_std_14)
target_link_libraries(flyft PUBLIC FFTW3::fftw3)
