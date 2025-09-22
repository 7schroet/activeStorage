add_executable(hdf5Writer hdf5Writer.c)
target_link_libraries(hdf5Writer PRIVATE HDF5::HDF5)
install(TARGETS hdf5Writer)
