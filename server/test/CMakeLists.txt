add_executable(argparse_test argparse_test.cpp)
target_link_libraries(argparse_test GTest::gtest_main as-rpc)
gtest_discover_tests(argparse_test)
