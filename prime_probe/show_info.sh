#!/bin/bash

# Function to process each entry
process_entry() {
  entry=$1
  echo "Processing entry: $entry"
  cat out.txt | grep "$entry"
}

# Check if any arguments are provided
if [ $# -eq 0 ]; then
  echo "Please provide at least one argument."
  exit 1
fi

# Iterate over each argument and process it
for arg in "$@"; do
  process_entry "$arg"
done

cat info.txt | grep average