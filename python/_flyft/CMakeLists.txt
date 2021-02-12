find_package(pybind11 CONFIG)

pybind11_add_module(_flyft
    _flyft.cc
    composite_functional.cc
    external_potential.cc
    field.cc
    free_energy_functional.cc
    hard_wall_potential.cc
    ideal_gas_functional.cc
    mesh.cc
    piccard_iteration.cc
    rosenfeld_fmt.cc
    solver.cc
    state.cc
    )

target_include_directories(
    _flyft
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
target_compile_features(_flyft PUBLIC cxx_std_14)
target_link_libraries(_flyft PUBLIC flyft)
