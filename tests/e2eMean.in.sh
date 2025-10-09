#!/usr/bin/env bash

# $1: the as-server executable
# $2: the as-client executable

set -ue

ADDRESS_FILE=e2eMeanAddress
PORT=8080
H5FILE=file.h5
RESULT=asrpc_results_file__dataset_mean.h5
MEAN_INPUTS=("011" "111")

$1 --addressfile $ADDRESS_FILE --port $PORT &
PID=$!

sleep 1

function cleanup(){
  rm -f "$RESULT" "$ADDRESS_FILE" "$H5FILE"
  kill $PID
}
trap cleanup EXIT

for input in "${MEAN_INPUTS[@]}"; do

  RESULT_REF="@CMAKE_CURRENT_SOURCE_DIR@/e2eMean${input}.h5"
  (for _ in $(seq 1 20); do echo ; done) | $2 --addressfile $ADDRESS_FILE --mean "$input"

  # h5diff is somewhat inconsistent. If the files can be diffed and there are
  # no differences, the exit code is 0 and there is not stdout. If there are
  # differences, the exit code is 1 and there is something on stdout. So far
  # everything works as expected, but if the files cannot be diffed, the exit code
  # is still 0. This case if obviously a failure. So for that case, we need to also
  # capture stdout and check if it is empty, alongside the exit code check.
  set +e
  h5diff_result=$(@HDF5_DIFF_EXECUTABLE@ -c "$RESULT" "$RESULT_REF")
  if [[ "$h5diff_result" || $? != 0 ]]; then
    echo "Failure while comparing to $RESULT_REF:"
    echo "$h5diff_result"

    COPY_ON_FAIL="./failure.h5"
    echo "Copying failed file to $(realpath $COPY_ON_FAIL)"
    cp "$RESULT" "$COPY_ON_FAIL"

    exit 1
  fi
  rm "$RESULT"
done
