#!/bin/bash

# **NOTE: Vibed coded like the rest of the cross-compiling stuff). Will manually improve later, though it'll do it for my own initial tests.**

# Test script for running Windows executables via Wine
#
# This script orchestrates the Docker-based Wine testing workflow:
# 1. Checks Docker availability
# 2. Builds Wine Docker image (if needed)
# 3. Runs Windows executable inside Wine container
# 4. Shows output

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PARENT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"  # micro-logger-cpp root
IMAGE_NAME="micro-logger-wine-test"
DOCKERFILE="Dockerfile.wine-test"

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Parse arguments
REBUILD=false

for arg in "$@"; do
    case $arg in
        --rebuild)
            REBUILD=true
            ;;
        --help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --rebuild   Rebuild Docker image even if it exists"
            echo "  --help      Show this help message"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $arg${NC}"
            echo "Run with --help for usage information"
            exit 1
            ;;
    esac
done

echo "========================================"
echo "micro-logger-cpp Wine Test"
echo "========================================"
echo ""
echo "Host: $(uname -s) $(uname -m)"
echo "Wine: 64-bit (wine64)"
echo ""

# Check if Docker is installed
if ! command -v docker &> /dev/null; then
    echo -e "${RED}❌ Error: Docker is not installed${NC}"
    echo "   Please install Docker Desktop: https://www.docker.com/products/docker-desktop"
    exit 1
fi

# Check if Docker is running
if ! docker info &> /dev/null; then
    echo -e "${RED}❌ Error: Docker is not running${NC}"
    echo "   Please start Docker Desktop"
    exit 1
fi

# Check if build-windows/ exists
if [ ! -d "$SCRIPT_DIR/build-windows" ]; then
    echo -e "${RED}❌ Error: build-windows/ directory not found${NC}"
    echo "   Please run ./cross-compile-windows.sh first to build Windows executables"
    exit 1
fi

# Check if .exe file exists
if [ ! -f "$SCRIPT_DIR/build-windows/micro_logger_sample.exe" ]; then
    echo -e "${RED}❌ Error: micro_logger_sample.exe not found${NC}"
    echo "   Please run ./cross-compile-windows.sh first to build Windows executables"
    exit 1
fi

# Build Docker image if needed
if [ "$REBUILD" = true ] || ! docker images | grep -q "$IMAGE_NAME"; then
    if [ "$REBUILD" = true ]; then
        echo "Rebuilding Docker image: $IMAGE_NAME"
    else
        echo "Building Docker image: $IMAGE_NAME"
        echo "(This may take a few minutes on first run)"
    fi
    echo ""

    docker build \
        -f "$SCRIPT_DIR/$DOCKERFILE" \
        -t "$IMAGE_NAME" \
        "$SCRIPT_DIR"

    if [ $? -eq 0 ]; then
        echo ""
        echo -e "${GREEN}✅ Docker image built successfully${NC}"
    else
        echo ""
        echo -e "${RED}❌ Docker image build failed${NC}"
        exit 1
    fi
else
    echo -e "${GREEN}✅ Docker image exists: $IMAGE_NAME${NC}"
    echo "   (Use --rebuild to rebuild the image)"
fi

echo ""
echo "Running Windows executable in Wine..."
echo ""
echo "========================================"

# Run Wine test
docker run --rm \
    -v "$PARENT_DIR:/src" \
    -w /src/cross-compile \
    "$IMAGE_NAME" \
    wine64 build-windows/micro_logger_sample.exe

# Check exit code
if [ $? -eq 0 ]; then
    echo "========================================"
    echo ""
    echo -e "${GREEN}🎉 Wine test successful!${NC}"
    echo ""
    echo "The Windows executable runs correctly under Wine emulation."
    echo ""
    echo "Next steps:"
    echo "  1. Copy cross-compile/build-windows/*.exe to a Windows machine"
    echo "  2. Test the executable on real Windows hardware"
else
    echo "========================================"
    echo ""
    echo -e "${RED}❌ Wine test failed${NC}"
    exit 1
fi
