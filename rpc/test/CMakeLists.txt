file(GLOB th_test_src th_*.cpp)

add_executable(th_integration_test ${th_test_src})
target_link_libraries(th_integration_test GTest::gtest_main as-rpc)
# Deliberately not using gtest_discover_tests because we need to ensure
# that each server is started on a different port. This is handled in
# the test source.
add_test(NAME th_integration_test
         COMMAND ./th_integration_test --gtest_shuffle
)
