# Use this function to add new tests.
# It receives two mandatory keywords args:
# - NAME : the name of the test case
# - SOURCES : a list of sources
#
# The other keyword arguments are optional:
# - INCLUDE_DIRS : other include directories that the test requires
function(add_check_test)
  set(one_value_args NAME)
  set(multi_value_args SOURCES INCLUDE_DIRS)
  cmake_parse_arguments(PARSE_ARGV 0 arg "" "${one_value_args}" "${multi_value_args}")

  if(NOT DEFINED arg_NAME)
    message(FATAL_ERROR "Must provide a name for the test via NAME!")
  endif()

  if(NOT DEFINED arg_SOURCES)
    message(FATAL_ERROR "Must provide sources for the test via SOURCES!")
  endif()

  add_executable("${arg_NAME}" "${arg_SOURCES}")
  target_link_libraries("${arg_NAME}" PRIVATE "${CHECK_LIBRARIES}")
  target_include_directories("${arg_NAME}" PRIVATE
    "${CHECK_INCLUDE_DIRS}" "${arg_INCLUDE_DIRS}")
  add_test(NAME "${arg_NAME}" COMMAND "${arg_NAME}")
endfunction()
