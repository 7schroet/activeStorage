#!/usr/bin/env bash

# Prints the used memory in bytes at an interval of 0.1 seconds.
# Throw this in the background to monitor memory usage on whichever device.

filename=$(hostname).mem_usage

while true; do
  echo "$(date +%s.%N): $(free -bL | tr -s ' ' | cut -d ' ' -f6)" >> "${filename}"
  sleep 0.1
done
