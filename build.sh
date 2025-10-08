#!/bin/bash
set -e

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

# Go to build directory
cd build

# Run CMake to configure the project
cmake ..

# Build the project
make

# Copy kernel file if needed (CMake does this, but ensure it's present)
cp ../hello.cl .

# Run the executable
./raytrace
