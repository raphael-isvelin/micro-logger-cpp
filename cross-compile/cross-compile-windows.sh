#!/bin/bash

# **NOTE: Vibed coded like the rest of the cross-compiling stuff). Will manually improve later, though it'll do it for my own initial tests.**

# Wrapper script for Docker-based cross-compilation
#
# This script orchestrates the Docker-based cross-compilation workflow:
# 1. Checks Docker availability
# 2. Builds Docker image (if needed)
# 3. Runs build-windows.sh inside container
# 4. Copies output back to host

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PARENT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"  # micro-logger-cpp root
IMAGE_NAME="micro-logger-windows-builder"
DOCKERFILE="Dockerfile.windows-cross"

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Parse arguments
CLEAN=false
REBUILD=false
SHELL=false

for arg in "$@"; do
    case $arg in
        --clean)
            CLEAN=true
            ;;
        --rebuild)
            REBUILD=true
            ;;
        --shell)
            SHELL=true
            ;;
        --help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --clean     Remove build-windows/ directory before compiling"
            echo "  --rebuild   Rebuild Docker image even if it exists"
            echo "  --shell     Open interactive shell in Docker container"
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
echo "micro-logger-cpp Windows Cross-Compilation"
echo "========================================"
echo ""
echo "Host: $(uname -s) $(uname -m)"
echo "Target: Windows x86_64"
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

# Clean build directory if requested
if [ "$CLEAN" = true ]; then
    echo "Cleaning build-windows/ directory..."
    rm -rf "$SCRIPT_DIR/build-windows"
    echo ""
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

# Interactive shell mode
if [ "$SHELL" = true ]; then
    echo "Opening interactive shell in Docker container..."
    echo "Working directory: /src/cross-compile"
    echo "Type 'exit' to leave the container"
    echo ""

    docker run --rm -it \
        -v "$PARENT_DIR:/src" \
        -w /src/cross-compile \
        "$IMAGE_NAME" \
        /bin/bash

    exit 0
fi

# Run cross-compilation
echo "Running cross-compilation..."
echo ""

docker run --rm \
    -v "$PARENT_DIR:/src" \
    -w /src/cross-compile \
    "$IMAGE_NAME" \
    ./build-windows.sh

# Check exit code
if [ $? -eq 0 ]; then
    echo ""
    echo -e "${GREEN}🎉 Cross-compilation successful!${NC}"
    echo ""
    echo "Windows executables are in: cross-compile/build-windows/"
    echo ""
    echo "Files created:"
    ls -lh "$SCRIPT_DIR/build-windows"/*.exe
    echo ""
    echo "Next steps:"
    echo "  1. Copy cross-compile/build-windows/*.exe to a Windows machine"
    echo "  2. Test the executable on Windows"
    echo ""
    echo "Or test with Wine:"
    echo "  ./test-windows-with-wine.sh"
else
    echo ""
    echo -e "${RED}❌ Cross-compilation failed${NC}"
    exit 1
fi
