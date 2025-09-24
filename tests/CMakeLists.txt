if(NOT HDF5_DIFF_EXECUTABLE)
  message(FATAL_ERROR "Did not find h5diff!")
else()
  message(STATUS "Found h5diff at ${HDF5_DIFF_EXECUTABLE}")
endif()

configure_file(e2eMean.in.sh e2eMean.sh @ONLY)
add_test(
  NAME e2eMean
  COMMAND ./e2eMean.sh $<TARGET_FILE:as-server> $<TARGET_FILE:as-client>
)
