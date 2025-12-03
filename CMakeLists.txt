cmake_minimum_required(VERSION 3.25)
project(ActiveStorage)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(ENABLE_SANITIZER "Build with asan, lsan and ubsan" OFF)
option(ENABLE_TESTS "Build with tests enabled (requires GoogleTest)" OFF)
option(ENABLE_COVERAGE "Create target 'coverage' to calculate test coverage (requires gcov and lcov)" OFF)
option(ENABLE_STATIC_ANALYSIS "Create target 'analysis' for static analyses (requires cppcheck and clang-tidy)" OFF)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "" FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "" FORCE)
  message(STATUS "No install prefix was specified, defaulting to ${CMAKE_CURRENT_SOURCE_DIR}/install")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(default_build "Release")
  message(STATUS "No build type was specified, defaulting to ${default_build}")
  set(CMAKE_BUILD_TYPE ${default_build} CACHE STRING "" FORCE)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_EXTENSIONS OFF)

find_package(thallium REQUIRED)
find_package(HDF5 COMPONENTS C;CXX REQUIRED)

add_compile_options("$<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Wshadow>")
add_compile_options("$<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wnrvo>")

if(ENABLE_SANITIZER)
  add_compile_options("-fsanitize=address,leak,undefined")
  add_link_options("-fsanitize=address,leak,undefined")
endif()

if(ENABLE_TESTS)
  include(CTest)
  find_package(GTest REQUIRED)
  enable_testing()
  include(GoogleTest)
endif()

if(ENABLE_COVERAGE)
  include(coverage)
endif()

if(ENABLE_STATIC_ANALYSIS)
  include(static_analysis)
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
add_subdirectory(client)
add_subdirectory(plugin)
if(ENABLE_TESTS)
  add_subdirectory(tests)
endif()
