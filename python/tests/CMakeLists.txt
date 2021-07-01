# python source files
set(FLYFT_PYTEST_SOURCES
    __init__.py
    conftest.py
    test_boublik_hard_sphere.py
    test_brownian_diffusive_flux.py
    test_composite_external_potential.py
    test_composite_flux.py
    test_composite_functional.py
    test_crank_nicolson_integrator.py
    test_explicit_euler_integrator.py
    test_exponential_wall_potential.py
    test_field.py
    test_grand_potential.py
    test_hard_wall_potential.py
    test_harmonic_wall_potential.py
    test_ideal_gas.py
    test_implicit_euler_integrator.py
    test_lennard_jones_93_wall_potential.py
    test_linear_potential.py
    test_mesh.py
    test_mirror.py
    test_parameter.py
    test_picard_iteration.py
    test_rosenfeld_fmt.py
    test_state.py
    test_virial_expansion.py
    )

# copy pytest files to build directory for testing (not strictly necessary)
foreach(file IN LISTS FLYFT_PYTEST_SOURCES)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${file}
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_LIST_DIR}/${file}
            ${CMAKE_CURRENT_BINARY_DIR}/${file}
        DEPENDS ${file}
        POST_BUILD
        COMMENT "Copying flyft tests/${file}"
        )
endforeach()
add_custom_target(python_flyft_tests ALL DEPENDS ${FLYFT_PYTEST_SOURCES})
