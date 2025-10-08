# RPC integration tests
add_executable(tl_integration_test tl_server_integration_test.cpp)
target_link_libraries(tl_integration_test GTest::gtest_main as-rpc)
# Deliberately not using gtest_discover_tests because we need to ensure
# that each server is started on a different port. This is handled in
# the test source.
add_test(NAME tl_integration_test
         COMMAND ./tl_integration_test --gtest_shuffle
)

# HDF5 helper functions unit tests
set(H5_TEST_INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(h5_helpers_test.in.cpp h5_helpers_test.cpp @ONLY)

add_executable(h5_helpers_test h5_helpers_test.cpp)
target_include_directories(h5_helpers_test PRIVATE ../include)
target_link_libraries(h5_helpers_test GTest::gtest_main HDF5::HDF5)
gtest_discover_tests(h5_helpers_test)

# Kernel math
add_executable(as_rpc_kernel_math_test
  as_rpc_kernel_math_test.cpp
  ../as_rpc_kernel_math.cpp
)
target_include_directories(as_rpc_kernel_math_test PRIVATE ../include)
target_link_libraries(as_rpc_kernel_math_test GTest::gtest_main HDF5::HDF5)
gtest_discover_tests(as_rpc_kernel_math_test)
