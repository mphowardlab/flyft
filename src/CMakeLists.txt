add_library(flyft SHARED
    composite_functional.cc
    external_potential.cc
    fourier_transform.cc
    free_energy_functional.cc
    hard_wall_potential.cc
    ideal_gas_functional.cc
    mesh.cc
    piccard_iteration.cc
    reciprocal_mesh.cc
    rosenfeld_fmt.cc
    solver.cc
    state.cc
    )

target_include_directories(
    flyft
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
target_compile_features(flyft PUBLIC cxx_std_14)
# compiler warnings for gcc / clang (hard code for now)
target_compile_options(flyft PRIVATE -Werror -Wall -Wextra)
target_link_libraries(flyft PUBLIC FFTW3::fftw3)
