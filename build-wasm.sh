#!/bin/bash

set -e

BUILD_DIR="${1:-build-wasm}"
BUILD_TYPE="${2:-Release}"

if [ -z "$EMSDK" ]; then
    echo "Error: EMSDK environment variable not set"
    echo "Please source emsdk_env.sh first"
    exit 1
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="${EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DWEBVULKAN_WASM=ON \
    -DWEBVULKAN_BUILD_SAMPLES=ON \
    -DWEBVULKAN_BUILD_TESTS=OFF

cmake --build . -j$(nproc)

echo "WASM build complete in $BUILD_DIR"
