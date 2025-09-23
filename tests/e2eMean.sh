#!/usr/bin/env bash

set -ue

ADDRESS_FILE=e2eMeanAddress
PORT=8080
RESULT=asrpc_results_file__dataset_mean.h5

$1 --addressfile $ADDRESS_FILE --port $PORT &
PID=$!

sleep 1

(for _ in $(seq 1 20); do echo ; done) | $2 --addressfile $ADDRESS_FILE

h5dump $RESULT
rm $RESULT $ADDRESS_FILE
kill $PID
