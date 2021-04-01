add_library(flyft SHARED
    composite_external_potential.cc
    composite_functional.cc
    external_potential.cc
    fourier_transform.cc
    free_energy_functional.cc
    grand_potential.cc
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
        $<INSTALL_INTERFACE:flyft/include>
    )
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
