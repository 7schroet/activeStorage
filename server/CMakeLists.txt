file(GLOB server_src *.cpp)

add_executable(as-server ${server_src})
target_link_libraries(as-server PRIVATE as-rpc)
target_link_libraries(as-server PRIVATE thallium)
install(TARGETS as-server)

if(ENABLE_TESTS)
  add_subdirectory(test)
endif()
