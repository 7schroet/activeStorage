cmake_minimum_required(VERSION 3.25)
project(ActiveStorage)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "" FORCE)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "" FORCE)
endif()

find_package(Mercury REQUIRED)

add_compile_options("$<$<C_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>")
add_subdirectory(server)
