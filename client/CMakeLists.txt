add_executable(as-client)
target_sources(
  as-client
  PRIVATE
    argparse.cpp
    main.cpp
  PRIVATE
    FILE_SET HEADERS
)

target_link_libraries(as-client PRIVATE as-rpc-flags)
target_link_libraries(
  as-client
  PRIVATE
    as-rpc
    HDF5::HDF5
)
if(CAN_USE_LTO)
  set_property(
    TARGET
      as-client
    PROPERTY
      INTERPROCEDURAL_OPTIMIZATION
        ON
  )
endif()
install(TARGETS as-client)

if(ENABLE_TESTS)
  add_subdirectory(test)
endif()
