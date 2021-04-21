# python source files
set(FLYFT_PY_SOURCES
    __init__.py
    external.py
    functional.py
    mirror.py
    solver.py
    state.py
    )

# pull in _flyft files here to get compiled module at same level in build as python module
set(_FLYFT_CC_SOURCES
    _flyft.cc
    composite_external_potential.cc
    composite_functional.cc
    external_potential.cc
    field.cc
    functional.cc
    grand_potential.cc
    hard_wall_potential.cc
    harmonic_wall_potential.cc
    ideal_gas_functional.cc
    lennard_jones_93_wall_potential.cc
    mesh.cc
    piccard_iteration.cc
    rosenfeld_fmt.cc
    solver.cc
    state.cc
    type_map.cc
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
