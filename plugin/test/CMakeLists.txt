set(script e2ePlugin.sh)
set(config_file config.toml)
configure_file(e2ePlugin.in.sh ${script} @ONLY)
configure_file(config.in.toml ${config_file} @ONLY)
add_test(
  NAME e2ePlugin
  COMMAND ./${script} $<TARGET_FILE:as-server> $<TARGET_FILE:hdf5Writer> ${config_file}
)
