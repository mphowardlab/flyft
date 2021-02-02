set(TEST_NAMES
    fourier_transform
    rosenfeld_fmt
)

foreach(TEST_NAME IN LISTS TEST_NAMES)
    add_executable(test_${TEST_NAME} test_${TEST_NAME}.cc)
    target_compile_features(test_${TEST_NAME} PRIVATE cxx_std_14)
    target_link_libraries(test_${TEST_NAME} PRIVATE flyft)
endforeach()
