option(ENABLE_PLUGIN_LOGGING "Enable log messages for the HDF5 plugin" OFF)

add_library(as-rpc-hdf5 SHARED)
target_sources(as-rpc-hdf5
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
  BASE_DIRS include
)
target_link_libraries(as-rpc-hdf5 PRIVATE as-rpc-flags)
target_link_libraries(as-rpc-hdf5 PRIVATE HDF5::HDF5 as-rpc)
if(ENABLE_PLUGIN_LOGGING)
  target_compile_definitions(as-rpc-hdf5 PRIVATE ENABLE_PLUGIN_LOGGING)
endif()
install(TARGETS as-rpc-hdf5)

add_executable(hdf5Writer hdf5Writer.c)
target_link_libraries(hdf5Writer PRIVATE as-rpc-flags)
target_link_libraries(hdf5Writer PRIVATE HDF5::HDF5)
install(TARGETS hdf5Writer)
