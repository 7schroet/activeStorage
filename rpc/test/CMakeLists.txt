file(GLOB tl_test_src tl_*.cpp)

add_executable(tl_integration_test ${tl_test_src})
target_link_libraries(tl_integration_test GTest::gtest_main as-rpc)
# Deliberately not using gtest_discover_tests because we need to ensure
# that each server is started on a different port. This is handled in
# the test source.
add_test(NAME tl_integration_test
         COMMAND ./tl_integration_test --gtest_shuffle
)
