#!/usr/bin/env bash

set -ue

ADDRESS_FILE=e2eMeanAddress
PORT=8080
H5FILE=file.h5
RESULT=asrpc_results_file__dataset_mean.h5
RESULT_REF="@CMAKE_CURRENT_SOURCE_DIR@/e2eMean.h5"

$1 --addressfile $ADDRESS_FILE --port $PORT &
PID=$!

sleep 1

(for _ in $(seq 1 20); do echo ; done) | $2 --addressfile $ADDRESS_FILE

function cleanup(){
  rm "$RESULT" "$ADDRESS_FILE" "$H5FILE"
  kill $PID
}
trap cleanup EXIT

h5diff_result=$(@HDF5_DIFF_EXECUTABLE@ -c "$RESULT" "$RESULT_REF")
if [[ "$h5diff_result" ]]; then
  echo "$h5diff_result"
  exit 1
fi
