#!/usr/bin/env bash

# $1: the as-server executable
# $2: the as-client executable

set -ue

server_exe=$1
client_exe=$2
script_dir="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

address_file=e2eMeanAddress
port=8080
client_output_stem="$script_dir/infile"
rpc_output_stem="$script_dir/rpc_outfile"
mean_inputs=("001" "010" "100" "101" "110" "111")

export HDF5_USE_FILE_LOCKING=FALSE

$server_exe --addressfile $address_file --port $port &
server_pid=$!

function check_server_health(){
  while :; do
    if [[ ! -e /proc/"$1" ]]; then
      kill -- $$
    fi
    sleep 0.5
  done
}
check_server_health $server_pid &
monitor_pid=$!
sleep 1 # await server startup

function cleanup(){
  rm -f "$rpc_output_stem"* "$address_file" "$client_output_stem"*
  kill $monitor_pid $server_pid
}
trap cleanup EXIT INT TERM

function run_test_case(){
  local mean_in=$1
  local type_in=$2
  local ref=$3

  local result_ref="@CMAKE_CURRENT_SOURCE_DIR@/e2eMean${ref}.h5"
  local client_output="${client_output_stem}_${mean_in}_${type_in}.h5"
  local rpc_output="${rpc_output_stem}_${mean_in}_${type_in}.h5"
  (
    for _ in $(seq 1 2); do
      for _ in $(seq 1 10); do
        echo
      done
      sleep 0.2
    done
   ) | $client_exe --addressfile $address_file --mean "$mean_in" --type "$type_in" --output "$client_output" --rpc-output "$rpc_output"

  # h5diff is somewhat inconsistent. If the files can be diffed and there are
  # no differences, the exit code is 0 and there is not stdout. If there are
  # differences, the exit code is 1 and there is something on stdout. So far
  # everything works as expected, but if the files cannot be diffed, the exit code
  # is still 0. This case if obviously a failure. So for that case, we need to also
  # capture stdout and check if it is empty, alongside the exit code check.
  set +e
  h5diff_result=$(@HDF5_DIFF_EXECUTABLE@ -c "$rpc_output" "$result_ref")
  if [[ "$h5diff_result" || $? != 0 ]]; then
    echo "Failure while comparing to $result_ref:"
    echo "$h5diff_result"

    local copy_on_fail="${script_dir}/failure.h5"
    echo "Copying failed file to $(realpath "$copy_on_fail")"
    cp "$rpc_output" "$copy_on_fail"

    exit 1
  fi
  set -e
}

for input in "${mean_inputs[@]}"; do
  run_test_case "$input" "double" "$input"
done

for input in "float" "int"; do
  run_test_case "011" "$input" "$input"
done
