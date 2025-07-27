file(GLOB client_src *.cpp)

add_executable(as-client ${client_src})
target_link_libraries(as-client PRIVATE as-rpc)
install(TARGETS as-client)

if(ENABLE_TESTS)
  add_subdirectory(test)
endif()
