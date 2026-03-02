find_program(CPPCHECK_EXE cppcheck REQUIRED)
set(cppcheck_dir ${CMAKE_BINARY_DIR}/cppcheck)
file(MAKE_DIRECTORY ${cppcheck_dir})
add_custom_target(
  cppcheck
  COMMAND
    ${CPPCHECK_EXE} --project=${CMAKE_BINARY_DIR}/compile_commands.json -itest
    -iexternals --disable=missingInclude
    --enable=warning,style,unusedFunction,information --check-level=exhaustive
    --inconclusive --error-exitcode=3 -j 4 --std=c++23
    --cppcheck-build-dir=${cppcheck_dir}
    --suppressions-list=${CMAKE_SOURCE_DIR}/.cppcheck_suppress.txt
)

find_program(CLANG_TIDY_EXE run-clang-tidy REQUIRED)
add_custom_target(
  clang-tidy
  COMMAND
    ${CLANG_TIDY_EXE} -config= -p ${CMAKE_BINARY_DIR} -allow-no-checks
)

add_custom_target(analysis)
add_dependencies(
  analysis
  clang-tidy
  cppcheck
)
