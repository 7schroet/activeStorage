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
    components/link.cpp
    components/object.cpp
    components/request.cpp
    components/utils.cpp
    components/wrap.cpp
    H5VLpassthru_ext.cpp

  PRIVATE
  FILE_SET HEADERS
  BASE_DIRS include
)
target_link_libraries(as-rpc-hdf5 PRIVATE HDF5::HDF5)
install(TARGETS as-rpc-hdf5)

add_executable(hdf5Writer hdf5Writer.c)
target_link_libraries(hdf5Writer PRIVATE HDF5::HDF5)
install(TARGETS hdf5Writer)
