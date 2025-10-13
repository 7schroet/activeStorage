#!/usr/bin/env bash

# $1: the as-server executable
# $2: the as-client executable

set -ue

SERVER_EXE=$1
CLIENT_EXE=$2

ADDRESS_FILE=e2eMeanAddress
PORT=8080
H5FILE=file.h5
RESULT=asrpc_results_file__dataset_mean.h5
MEAN_INPUTS=("001" "010" "011" "100" "101" "110" "111")

$SERVER_EXE --addressfile $ADDRESS_FILE --port $PORT &
PID=$!

sleep 1

function cleanup(){
  rm -f "$RESULT" "$ADDRESS_FILE" "$H5FILE"
  kill $PID
}
trap cleanup EXIT

function run_test_case(){
  local mean_in=$1
  local type_in=$2

  local result_ref="@CMAKE_CURRENT_SOURCE_DIR@/e2eMean${input}.h5"
  (for _ in $(seq 1 20); do echo ; done) | $CLIENT_EXE --addressfile $ADDRESS_FILE --mean "$mean_in" --type "$type_in"

  # h5diff is somewhat inconsistent. If the files can be diffed and there are
  # no differences, the exit code is 0 and there is not stdout. If there are
  # differences, the exit code is 1 and there is something on stdout. So far
  # everything works as expected, but if the files cannot be diffed, the exit code
  # is still 0. This case if obviously a failure. So for that case, we need to also
  # capture stdout and check if it is empty, alongside the exit code check.
  set +e
  h5diff_result=$(@HDF5_DIFF_EXECUTABLE@ -c "$RESULT" "$result_ref")
  if [[ "$h5diff_result" || $? != 0 ]]; then
    echo "Failure while comparing to $result_ref:"
    echo "$h5diff_result"

    local copy_on_fail="./failure.h5"
    echo "Copying failed file to $(realpath $copy_on_fail)"
    cp "$RESULT" "$copy_on_fail"

    exit 1
  fi
  set -e
  rm "$RESULT"
}

for input in "${MEAN_INPUTS[@]}"; do
  run_test_case "$input" "double"
done

for input in "float" "int"; do
  run_test_case "011" "$input"
done
