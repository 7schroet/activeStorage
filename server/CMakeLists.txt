file(GLOB server_src *.c)

add_executable(as-server ${server_src})
target_link_libraries(as-server PRIVATE as-rpc)
install(PROGRAMS $<TARGET_FILE:as-server> TYPE BIN)

if(ENABLE_TESTS)
  add_subdirectory(test)
endif()
