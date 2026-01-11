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

add_library(as-rpc-config-parse STATIC)
target_sources(as-rpc-config-parse
  PRIVATE
    as_rpc_config_parse.cpp

  PUBLIC
  FILE_SET HEADERS
  BASE_DIRS
    include
    ${CMAKE_SOURCE_DIR}/rpc/include
  FILES
    include/as_rpc_config_parse.hpp
    ${CMAKE_SOURCE_DIR}/rpc/include/as_rpc_kernels.hpp
)
target_link_libraries(as-rpc-config-parse PRIVATE as-rpc-flags)
target_link_libraries(as-rpc-config-parse PRIVATE toml11::toml11)
install(TARGETS as-rpc-config-parse FILE_SET HEADERS)

add_executable(as-rpc-config-validate as_rpc_config_validate.cpp)
target_link_libraries(as-rpc-config-validate PRIVATE as-rpc-flags)
target_link_libraries(as-rpc-config-validate PRIVATE as-rpc-config-parse)
install(TARGETS as-rpc-config-validate)

if(ENABLE_TESTS)
  add_subdirectory(test)
endif()
