#!/bin/bash
# Build script for IDR Analysis C++ Project

set -e

echo "================================"
echo "IDR Analysis - Build Script"
echo "================================"
echo ""

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir -p build
fi

# Navigate to build directory
cd build

# Run CMake
echo "Running CMake..."
cmake ..

# Build
echo ""
echo "Building..."
make -j$(nproc)

echo ""
echo "================================"
echo "✓ Build Complete!"
echo "================================"
echo ""
echo "Executable location: $(pwd)/bin/idr_analysis"
echo "Test executable:     $(pwd)/bin/run_tests (if tests enabled)"
echo ""
echo "To run the program:"
echo "  ./bin/idr_analysis file1.narrowPeak file2.narrowPeak"
echo ""
echo "To run tests:"
echo "  ./bin/run_tests"
echo ""
