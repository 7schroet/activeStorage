add_cmocka_test(argparse_test
  SOURCES argparse_test.c
  LINK_OPTIONS "-Wl,--wrap=exit"
)
target_link_libraries(argparse_test PRIVATE cmocka::cmocka)
target_include_directories(argparse_test PRIVATE "${CMAKE_SOURCE_DIR}/rpc/include")
