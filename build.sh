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

# Copy assets to the build directory
if [ -d "../assets" ]; then
    cp -r ../assets .
fi

# Copy saves to the build directory
if [ -d "../saves" ]; then
    cp -r ../saves .
fi

# Run the executable (it's now in bin/ subdirectory)
./bin/raytrace
