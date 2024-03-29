add_library(flyft SHARED
    boublik_hard_sphere_functional.cc
    brownian_diffusive_flux.cc
    cartesian_mesh.cc
    composite_external_potential.cc
    composite_flux.cc
    composite_functional.cc
    communicator.cc
    crank_nicolson_integrator.cc
    data_layout.cc
    explicit_euler_integrator.cc
    exponential_wall_potential.cc
    external_potential.cc
    flux.cc
    fourier_transform.cc
    functional.cc
    grand_potential.cc
    grid_interpolator.cc
    hard_wall_potential.cc
    harmonic_wall_potential.cc
    ideal_gas_functional.cc
    implicit_euler_integrator.cc
    integrator.cc
    lennard_jones_93_wall_potential.cc
    linear_potential.cc
    mesh.cc
    parallel_mesh.cc
    picard_iteration.cc
    rosenfeld_fmt.cc
    rpy_diffusive_flux.cc
    solver.cc
    spherical_mesh.cc
    state.cc
    three_dimensional_index.cc
    tracked_object.cc
    virial_expansion.cc
    white_bear.cc
    white_bear_mark_ii.cc
    )

target_include_directories(
    flyft
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:flyft/include>
    )
target_compile_definitions(flyft PUBLIC _USE_MATH_DEFINES)
if(FLYFT_MPI)
    target_link_libraries(flyft PUBLIC MPI::MPI_CXX)
    target_compile_definitions(flyft PUBLIC FLYFT_MPI)
endif()
if(FLYFT_OPENMP)
    target_link_libraries(flyft PUBLIC OpenMP::OpenMP_CXX FFTW3::fftw3_omp)
    target_compile_definitions(flyft PUBLIC FLYFT_OPENMP)
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
