add_test(
  NAME e2eMean
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/e2eMean.sh
    $<TARGET_FILE:as-server> $<TARGET_FILE:as-client>
    ${CMAKE_CURRENT_SOURCE_DIR}/e2eMean.h5
)
