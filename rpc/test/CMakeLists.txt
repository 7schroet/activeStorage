# RPC integration tests
add_executable(tl_integration_test tl_server_integration_test.cpp)
target_link_libraries(tl_integration_test PRIVATE as-rpc-flags)
target_link_libraries(tl_integration_test PRIVATE GTest::gtest_main as-rpc)
# Deliberately not using gtest_discover_tests because we need to ensure
# that each server is started on a different port. This is handled in
# the test source.
add_test(NAME tl_integration_test
         COMMAND ./tl_integration_test --gtest_shuffle
)

# HDF5 helper functions unit tests
set(H5_TEST_INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(h5_helpers_test.in.cpp h5_helpers_test.cpp @ONLY)
# Required to make clang-tidy actually ignore the
# newly created test file as intended.
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(h5_helpers_test h5_helpers_test.cpp)
target_include_directories(h5_helpers_test PRIVATE ../include)
target_link_libraries(h5_helpers_test PRIVATE as-rpc-flags)
target_link_libraries(h5_helpers_test PRIVATE GTest::gtest_main HDF5::HDF5)
gtest_discover_tests(h5_helpers_test)

# Kernel math
add_executable(as-rpc-kernel-math-test
  as_rpc_kernel_math_test.cpp
  ../as_rpc_kernel_math.cpp
)
target_include_directories(as-rpc-kernel-math-test PRIVATE ../include)
target_link_libraries(as-rpc-kernel-math-test PRIVATE as-rpc-flags)
target_link_libraries(as-rpc-kernel-math-test PRIVATE GTest::gtest_main HDF5::HDF5)
gtest_discover_tests(as-rpc-kernel-math-test)
