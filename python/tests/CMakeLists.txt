# python source files
set(FLYFT_PYTEST_SOURCES
    __init__.py
    test_mesh.py
    test_field.py
    test_mirror.py
    test_state.py
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
