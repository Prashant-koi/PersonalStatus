#!/bin/bash
# Linux build script

# Install dependencies on Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential cmake libx11-dev

# Create build directory
mkdir -p build
cd build

# Configure and build
cmake ..
make

echo "Build complete! Run with: ./personal_status_monitor"