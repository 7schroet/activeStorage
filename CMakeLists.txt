cmake_minimum_required(VERSION 3.25)
project(ActiveStorage)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "" FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "" FORCE)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(default_build "Release")
  message(STATUS "No build type was specified, defaulting to ${default_build}")
  set(CMAKE_BUILD_TYPE ${default_build} CACHE STRING "" FORCE)
endif()

find_package(Mercury REQUIRED)

add_compile_options("$<$<C_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>")
add_subdirectory(server)
