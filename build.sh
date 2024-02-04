#!/bin/bash

# Check if gcc installed
if ! [ -x "$(command -v gcc)" ]; then
  echo 'Error: gcc is not installed.' >&2
  exit 1
else 
  echo 'gcc is installed.'
fi

echo "Building the project..."

# Create the binary file
gcc -o copr copr.c -Wall -Wformat=0

# add the binary file to usr/local/bin
sudo mv copr /usr/local/bin

echo "Built successfully."
