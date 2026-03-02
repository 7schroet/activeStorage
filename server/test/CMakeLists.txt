add_executable(server_argparse_test argparse_test.cpp)
target_link_libraries(server_argparse_test PRIVATE as-rpc-flags)
target_link_libraries(
  server_argparse_test
  PRIVATE
    GTest::gtest_main
    as-rpc
)
gtest_discover_tests(server_argparse_test)
