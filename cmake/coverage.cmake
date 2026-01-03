find_program(GCOV_EXE gcov REQUIRED)
find_program(LCOV_EXE lcov REQUIRED)
find_program(GENHTML_EXE genhtml REQUIRED)

if(NOT ENABLE_TESTS)
  message(FATAL_ERROR "Coverage requires tests to be enabled!")
endif()

if(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
  message(FATAL_ERROR "Coverage requires gcc!")
endif()

message(STATUS "Overriding build type to Debug for coverage calculation")
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)

target_compile_options(as-rpc-flags INTERFACE "--coverage")
target_link_options(as-rpc-flags INTERFACE "--coverage")

set(exclude_patterns
  "c++"
  "cereal"
  "gmock"
  "gtest"
  "hdf5"
  "mercury"
  "mochi"
  "/test"
)
list(TRANSFORM exclude_patterns PREPEND "--exclude;" OUTPUT_VARIABLE exclude_patterns)

add_custom_target(coverage
  COMMAND ${CMAKE_CTEST_COMMAND} -T Test -T Coverage
  COMMAND ${LCOV_EXE} -d . -b . --capture
            --output-file coverage.info
            --ignore-errors mismatch,mismatch
            ${exclude_patterns}
  COMMAND ${GENHTML_EXE} --demangle-cpp -o coverage coverage.info
)
