add_executable(parse-test main.cpp)
target_include_directories(parse-test PRIVATE ${CMAKE_SOURCE_DIR}/rpc/include)

set(ext_toml11_dir ${CMAKE_SOURCE_DIR}/externals/toml11)
set(ext_toml11_patch ${CMAKE_SOURCE_DIR}/externals/toml11.patch)

find_package(Git REQUIRED)
set(patch_check "${GIT_EXECUTABLE} apply ${ext_toml11_patch} --check -q")
set(patch_apply "${GIT_EXECUTABLE} apply ${ext_toml11_patch}")
execute_process(
  COMMAND bash -c "${patch_check} && ${patch_apply}"
  WORKING_DIRECTORY ${ext_toml11_dir}
)

set(TOML11_PRECOMPILE ON)
add_subdirectory("${ext_toml11_dir}" ${CMAKE_BINARY_DIR}/externals/toml11)
target_link_libraries(parse-test PRIVATE as-rpc-flags)
target_link_libraries(parse-test PUBLIC toml11::toml11)
