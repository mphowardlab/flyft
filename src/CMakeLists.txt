add_library(flyft SHARED
    boublik_hard_sphere_functional.cc
    brownian_diffusive_flux.cc
    communicator.cc
    composite_external_potential.cc
    composite_flux.cc
    composite_functional.cc
    crank_nicolson_integrator.cc
    data_layout.cc
    diffusive_flux.cc
    explicit_euler_integrator.cc
    exponential_wall_potential.cc
    external_potential.cc
    flux.cc
    fourier_transform.cc
    functional.cc
    grand_potential.cc
    hard_wall_potential.cc
    harmonic_wall_potential.cc
    ideal_gas_functional.cc
    implicit_euler_integrator.cc
    integrator.cc
    lennard_jones_93_wall_potential.cc
    linear_potential.cc
    mesh.cc
    picard_iteration.cc
    reciprocal_mesh.cc
    rosenfeld_fmt.cc
    solver.cc
    state.cc
    virial_expansion.cc
    wall_potential.cc
    )

target_include_directories(
    flyft
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:flyft/include>
    )
if(FLYFT_OPENMP)
    target_link_libraries(flyft PRIVATE OpenMP::OpenMP_CXX FFTW3::fftw3_omp)
    target_compile_definitions(flyft PRIVATE FLYFT_OPENMP)
endif()
target_link_libraries(flyft PUBLIC FFTW3::fftw3)
target_compile_features(flyft PUBLIC cxx_std_14)
# selectively turn on compile options for gcc & clang
target_compile_options(flyft PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Werror -Wall -Wextra>
     $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-fcolor-diagnostics>)

# install the library into "standard" names within flyft directory
install(TARGETS flyft LIBRARY DESTINATION flyft/lib)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/flyft DESTINATION flyft/include)
