option(ENABLE_PLUGIN_LOGGING "Enable log messages for the HDF5 plugin" OFF)

set(
  libs
  as-rpc-hdf5
  as-rpc-hdf5-bulk
)
add_library(as-rpc-hdf5 SHARED)
add_library(as-rpc-hdf5-bulk SHARED)
target_compile_definitions(as-rpc-hdf5 PRIVATE FILE_STAGING)
foreach(lib IN LISTS libs)
  target_sources(
    ${lib}
    PRIVATE
      components/attribute.cpp
      components/blob.cpp
      components/dataset.cpp
      components/datatype.cpp
      components/file.cpp
      components/group.cpp
      components/info.cpp
      components/init.cpp
      components/link.cpp
      components/log.cpp
      components/object.cpp
      components/optional.cpp
      components/request.cpp
      components/rpc_handler.cpp
      components/token.cpp
      components/utils.cpp
      components/wrap.cpp
      as_rpc_hdf5.cpp
    PRIVATE
      FILE_SET HEADERS
        BASE_DIRS
          include
  )
  target_link_libraries(${lib} PRIVATE as-rpc-flags)
  target_link_libraries(
    ${lib}
    PRIVATE
      HDF5::HDF5
      as-rpc
      as-rpc-config-parse
  )
  if(ENABLE_PLUGIN_LOGGING)
    target_compile_definitions(${lib} PRIVATE ENABLE_PLUGIN_LOGGING)
  endif()
  install(TARGETS ${lib})
endforeach()

add_executable(hdf5Writer hdf5Writer.c)
target_link_libraries(hdf5Writer PRIVATE as-rpc-flags)
target_link_libraries(hdf5Writer PRIVATE HDF5::HDF5)
install(TARGETS hdf5Writer)

add_executable(hdf5Writer_bench hdf5Writer_bench.c)
target_link_libraries(hdf5Writer_bench PRIVATE as-rpc-flags)
target_link_libraries(hdf5Writer_bench PRIVATE HDF5::HDF5)
install(TARGETS hdf5Writer_bench)

if(ENABLE_TESTS)
  add_subdirectory(test)
endif()
