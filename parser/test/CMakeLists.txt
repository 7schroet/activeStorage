add_executable(as-rpc-toml-test as_rpc_toml_test.cpp)
target_link_libraries(as-rpc-toml-test PRIVATE as-rpc-flags)
target_link_libraries(as-rpc-toml-test PRIVATE GTest::gtest_main as-rpc-config-parse)
gtest_discover_tests(as-rpc-toml-test)
