set(config_suffix "config.toml")
set(script_suffix "e2ePlugin.sh")

foreach(threads IN ITEMS 1 2)
  set(hdf5_writer_out "${CMAKE_CURRENT_BINARY_DIR}/dummyWrite_${threads}.h5")
  set(plugin_out_mean "${CMAKE_CURRENT_BINARY_DIR}/outmean_${threads}.h5")
  set(plugin_out_min "${CMAKE_CURRENT_BINARY_DIR}/outmin_${threads}.h5")
  set(plugin_out_max "${CMAKE_CURRENT_BINARY_DIR}/outmax_${threads}.h5")
  configure_file(config.in.toml "${threads}_${config_suffix}" @ONLY)
  configure_file(e2ePlugin.in.sh "${threads}_${script_suffix}" @ONLY)
endforeach()

add_test(
  NAME e2ePlugin
  COMMAND
    ./1_${script_suffix} $<TARGET_FILE:as-server> $<TARGET_FILE:hdf5Writer>
    1_${config_suffix}
)

add_test(
  NAME e2ePluginThreaded
  COMMAND
    env AS_RPC_THREADS=2 ./2_${script_suffix} $<TARGET_FILE:as-server>
    $<TARGET_FILE:hdf5Writer> 2_${config_suffix}
)
