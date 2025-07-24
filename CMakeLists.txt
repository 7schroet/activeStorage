cmake_minimum_required(VERSION 3.25)
project(ActiveStorage)

option(ENABLE_SANITIZER "Build with asan and ubsan" OFF)
option(ENABLE_STATIC_ANALYSIS "Build with -fanalyzer (requires gcc)" OFF)
option(ENABLE_TESTS "Build with tests enabled (requires Catch2)" OFF)

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

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(thallium REQUIRED)

add_compile_options("$<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>")

if(ENABLE_SANITIZER)
  add_compile_options("-fsanitize=address,undefined")
  add_link_options("-fsanitize=address,undefined")
endif()

if(ENABLE_STATIC_ANALYSIS)
  if(ENABLE_SANITIZER)
    message(FATAL_ERROR "Cannot build with both static analysis and sanitizers enabled!")
  endif()
  if(NOT CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    message(FATAL_ERROR "Only supporting static analysis with gcc!")
  endif()
  add_compile_options("-O0;-fanalyzer")
endif()

if(ENABLE_TESTS)
  find_package(GTest REQUIRED)
  enable_testing()
  include(GoogleTest)
endif()

include(GNUInstallDirs)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON CACHE BOOL "" FORCE)
file(RELATIVE_PATH lib_dir
  "${CMAKE_INSTALL_FULL_BINDIR}"
  "${CMAKE_INSTALL_FULL_LIBDIR}"
)

set(CMAKE_INSTALL_RPATH "$ORIGIN/${lib_dir};${CMAKE_INSTALL_RPATH}" CACHE STRING "" FORCE)

add_subdirectory(rpc)
add_subdirectory(server)
# add_subdirectory(client)
