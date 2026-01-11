add_executable(as-rpc-toml-test as_rpc_toml_test.cpp)
target_link_libraries(as-rpc-toml-test PRIVATE as-rpc-flags)
target_link_libraries(as-rpc-toml-test PRIVATE GTest::gtest_main as-rpc-config-parse)
gtest_discover_tests(as-rpc-toml-test)

add_test(NAME toml-validation-pass
  COMMAND as-rpc-config-validate ${CMAKE_CURRENT_SOURCE_DIR}/valid.toml
)

add_test(NAME toml-validation-missing-input
  COMMAND as-rpc-config-validate
)
set_tests_properties(toml-validation-missing-input PROPERTIES
  PASS_REGULAR_EXPRESSION "Path to toml file required!"
)

add_test(NAME toml-validation-fail
  COMMAND as-rpc-config-validate ${CMAKE_CURRENT_SOURCE_DIR}/invalid.toml
)
set_tests_properties(toml-validation-fail PROPERTIES
  PASS_REGULAR_EXPRESSION "key \"type\" not found"
)
