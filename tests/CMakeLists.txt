set(script e2eMean.sh)
configure_file(e2eMean.in.sh ${script} @ONLY)
add_test(
  NAME e2eMean
  COMMAND ./${script} $<TARGET_FILE:as-server> $<TARGET_FILE:as-client>
)
