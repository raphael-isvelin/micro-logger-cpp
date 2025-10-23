#!/bin/bash

# **NOTE: Vibed coded like the rest of the cross-compiling stuff). Will manually improve later, though it'll do it for my own initial tests.**

# Build script for cross-compiling micro-logger-cpp sample to Windows
#
# This script runs inside the Docker container to compile the sample for Windows.
# It can also be run directly on a system with mingw-w64 installed.

set -e

echo "========================================"
echo "Cross-Compiling micro-logger-cpp for Windows"
echo "========================================"
echo ""

# Detect if we're in Docker or native
if [ -f /.dockerenv ]; then
    echo "Running inside Docker container"
    IN_DOCKER=true
else
    echo "Running on host system"
    IN_DOCKER=false
fi

# Set up compiler
if [ "$IN_DOCKER" = true ]; then
    # In Docker, use posix variant for std::mutex support
    COMPILER="x86_64-w64-mingw32-g++-posix"
    TOOLCHAIN_FILE="/toolchain-windows.cmake"
else
    # Try to find mingw compiler on host (prefer posix variant)
    if command -v x86_64-w64-mingw32-g++-posix &> /dev/null; then
        COMPILER="x86_64-w64-mingw32-g++-posix"
    elif command -v x86_64-w64-mingw32-g++ &> /dev/null; then
        COMPILER="x86_64-w64-mingw32-g++"
        echo "⚠️  Warning: Using win32 threading model (std::mutex not supported)"
        echo "   Install mingw-w64-posix for full C++11 threading support"
    else
        echo "❌ Error: mingw-w64 compiler not found"
        echo "   Please install mingw-w64 or use Docker"
        exit 1
    fi
fi

echo "Compiler: $COMPILER"
echo ""

# Create output directory
OUTPUT_DIR="build-windows"
mkdir -p "$OUTPUT_DIR"

# Compilation flags
CXX_FLAGS="-std=c++20 -Wall -Wextra -O2 -static-libgcc -static-libstdc++ -static"
INCLUDE_DIR="../inc"

echo "Compiling sample for Windows..."
echo ""

# Compile micro_logger_sample.cpp
echo "  [1/1] micro_logger_sample.exe"
$COMPILER $CXX_FLAGS \
    sample/micro_logger_sample.cpp \
    -o "$OUTPUT_DIR/micro_logger_sample.exe" \
    -I"$INCLUDE_DIR"

if [ $? -eq 0 ]; then
    echo "        ✅ Success"
else
    echo "        ❌ Failed"
    exit 1
fi

echo ""
echo "========================================"
echo "Cross-compilation complete!"
echo "========================================"
echo ""
echo "Output directory: $OUTPUT_DIR/"
echo ""
ls -lh "$OUTPUT_DIR"/*.exe
echo ""
echo "To test on Windows:"
echo "  1. Copy $OUTPUT_DIR/*.exe to a Windows machine"
echo "  2. Run: micro_logger_sample.exe"
echo ""
echo "Note: This is a header-only library, so no linking issues expected"
