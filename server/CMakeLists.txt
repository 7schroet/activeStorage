add_executable(as-server)
target_sources(as-server
  PRIVATE
    argparse.cpp
    main.cpp

  PRIVATE
  FILE_SET HEADERS
)
target_link_libraries(as-server PRIVATE as-rpc-flags)
target_link_libraries(as-server PRIVATE as-rpc)
install(TARGETS as-server)

if(ENABLE_TESTS)
  add_subdirectory(test)
endif()
