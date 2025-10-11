find_program(CPPCHECK_EXE cppcheck REQUIRED)
add_custom_target(cppcheck
  COMMAND ${CPPCHECK_EXE}
    --project=${CMAKE_BINARY_DIR}/compile_commands.json
    -itest
    --disable=missingInclude
    --enable=warning,style,unusedFunction,information
    --check-level=exhaustive
    --inconclusive
    --error-exitcode=3
    -j 4
    --suppressions-list=${CMAKE_SOURCE_DIR}/cppcheck_suppress.txt
)

find_program(CLANG_TIDY_EXE run-clang-tidy)
add_custom_target(clang-tidy
  COMMAND ${CLANG_TIDY_EXE}
    -config=
    -p ${CMAKE_BINARY_DIR}
    -allow-no-checks
)
