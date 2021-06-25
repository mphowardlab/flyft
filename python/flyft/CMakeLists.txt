# python source files
set(FLYFT_PY_SOURCES
    __init__.py
    dynamics.py
    external.py
    functional.py
    mirror.py
    parameter.py
    solver.py
    state.py
    )

# pull in _flyft files here to get compiled module at same level in build as python module
set(_FLYFT_CC_SOURCES
    _flyft.cc
    boublik_hard_sphere_functional.cc
    brownian_diffusive_flux.cc
    composite_external_potential.cc
    composite_flux.cc
    composite_functional.cc
    crank_nicolson_integrator.cc
    diffusive_flux.cc
    explicit_euler_integrator.cc
    exponential_wall_potential.cc
    external_potential.cc
    field.cc
    flux.cc
    functional.cc
    grand_potential.cc
    hard_wall_potential.cc
    harmonic_wall_potential.cc
    ideal_gas_functional.cc
    integrator.cc
    lennard_jones_93_wall_potential.cc
    linear_potential.cc
    mesh.cc
    pair_map.cc
    parameter.cc
    picard_iteration.cc
    rosenfeld_fmt.cc
    solver.cc
    state.cc
    type_map.cc
    virial_expansion.cc
    wall_potential.cc
    )
list(TRANSFORM _FLYFT_CC_SOURCES PREPEND _flyft/)

# build _flyft extension module
pybind11_add_module(_flyft ${_FLYFT_CC_SOURCES})
target_include_directories(
    _flyft
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
target_compile_features(_flyft PUBLIC cxx_std_14)
target_link_libraries(_flyft PUBLIC flyft)
if (APPLE)
set_target_properties(_flyft PROPERTIES INSTALL_RPATH "@loader_path/lib")
else()
set_target_properties(_flyft PROPERTIES INSTALL_RPATH "\$ORIGIN/lib")
endif()

# copy python sources to build directory for testing (not strictly necessary)
foreach(file IN LISTS FLYFT_PY_SOURCES)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${file}
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_LIST_DIR}/${file}
            ${CMAKE_CURRENT_BINARY_DIR}/${file}
        DEPENDS ${file}
        POST_BUILD
        COMMENT "Copying flyft/${file}"
        )
endforeach()
add_custom_target(python_flyft ALL DEPENDS ${FLYFT_PY_SOURCES})

# install both python libraries into same spot
install(FILES ${FLYFT_PY_SOURCES} DESTINATION flyft)
install(TARGETS _flyft LIBRARY DESTINATION flyft)
