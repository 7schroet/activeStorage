#!/usr/bin/env bash

# $1: the as-server executable
# $2: the h5 writer executable
# $3: the name of the config file

set -ue

SERVER_EXE=$1
H5_WRITER_EXE=$2
CONFIG_NAME=$3

ADDRESS_FILE=e2ePluginAddress
PORT=8081
H5FILE="@CMAKE_CURRENT_BINARY_DIR@/dummyWrite.h5"
RESULT=asrpc_results_dummyWrite_dummyDataset_mean.h5

export HDF5_USE_FILE_LOCKING=FALSE

$SERVER_EXE --addressfile $ADDRESS_FILE --port $PORT &
PID=$!

sleep 1

function cleanup(){
  rm -f "$RESULT" "$ADDRESS_FILE" "$H5FILE"
  kill $PID
}
trap cleanup EXIT

HDF5_PLUGIN_PATH=$(realpath "@CMAKE_CURRENT_BINARY_DIR@/.."); export HDF5_PLUGIN_PATH
export HDF5_VOL_CONNECTOR="as-rpc-hdf5 under_vol=0;under_info={};"

export AS_RPC_SERVER_ADDRESS=$PWD/${ADDRESS_FILE}
export AS_RPC_OPERATIONS="@CMAKE_CURRENT_BINARY_DIR@/$CONFIG_NAME"

(
  for _ in $(seq 1 2); do
    for _ in $(seq 1 10); do
      echo
    done
    sleep 0.1
  done
) | $H5_WRITER_EXE

# see e2eMean.in.sh for explanation
set +e
unset HDF5_VOL_CONNECTOR
result_ref="@CMAKE_CURRENT_SOURCE_DIR@/e2ePluginRef.h5"
h5diff_result=$(@HDF5_DIFF_EXECUTABLE@ -d 0.5 -c "$RESULT" "$result_ref")
if [[ "$h5diff_result" || $? != 0 ]]; then
  echo "Failure while comparing to $result_ref:"
  echo "$h5diff_result"

  copy_on_fail="./failure.h5"
  echo "Copying failed file to $(realpath $copy_on_fail)"
  cp "$RESULT" "$copy_on_fail"

  exit 1
fi
set -e
