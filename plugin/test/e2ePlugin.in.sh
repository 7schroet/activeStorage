#!/usr/bin/env bash

# $1: the as-server executable
# $2: the h5 writer executable
# $3: the name of the config file

set -ue

server_exe=$1
h5_writer_exe=$2
config_name=$3

address_file=e2ePluginAddress
port=8081
h5file="@CMAKE_CURRENT_BINARY_DIR@/dummyWrite.h5"

result_mean="@CMAKE_CURRENT_BINARY_DIR@/outmean.h5"
result_max="@CMAKE_CURRENT_BINARY_DIR@/outmax.h5"
result_min="@CMAKE_CURRENT_BINARY_DIR@/outmin.h5"
result_ref_mean="@CMAKE_CURRENT_SOURCE_DIR@/e2ePluginMeanRef.h5"
result_ref_max="@CMAKE_CURRENT_SOURCE_DIR@/e2ePluginMaxRef.h5"
result_ref_min="@CMAKE_CURRENT_SOURCE_DIR@/e2ePluginMinRef.h5"

export HDF5_USE_FILE_LOCKING=FALSE

$server_exe --addressfile $address_file --port $port &
PID=$!

sleep 1

function cleanup(){
  rm -f "$result_mean" "$result_max" "$result_min" "$address_file" "$h5file"
  kill $PID
}
trap cleanup EXIT

HDF5_PLUGIN_PATH=$(realpath "@CMAKE_CURRENT_BINARY_DIR@/.."); export HDF5_PLUGIN_PATH
export HDF5_VOL_CONNECTOR="as-rpc-hdf5 under_vol=0;under_info={};"

export AS_RPC_SERVER_ADDRESS=$PWD/${address_file}
export AS_RPC_OPERATIONS="@CMAKE_CURRENT_BINARY_DIR@/$config_name"

(
  for _ in $(seq 1 2); do
    for _ in $(seq 1 10); do
      echo
    done
    sleep 0.1
  done
) | $h5_writer_exe

# see e2eMean.in.sh for explanation
set +e
unset HDF5_VOL_CONNECTOR
for op in "max" "mean" "min"; do
  expected="result_ref_${op}"
  actual="result_${op}"
  h5diff_result=$(@HDF5_DIFF_EXECUTABLE@ -c "${!actual}" "${!expected}")
  if [[ "$h5diff_result" || $? != 0 ]]; then
    echo "Failure while comparing to ${!expected}:"
    echo "$h5diff_result"

    copy_on_fail="./failure.h5"
    echo "Copying failed file to $(realpath $copy_on_fail)"
    cp "${!actual}" "$copy_on_fail"

    exit 1
  fi
done
set -e
