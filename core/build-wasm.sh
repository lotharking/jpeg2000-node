#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
OPENJPEG_DIR="$REPO_ROOT/openjpeg-wasm"
OUT_DIR="$REPO_ROOT/lib/src/wasm"

echo "==> Checking Emscripten..."
if ! command -v emcc &>/dev/null; then
  echo "Error: emcc not found. Activate the Emscripten SDK first:"
  echo "  source /path/to/emsdk/emsdk_env.sh"
  exit 1
fi
emcc --version

# Build OpenJPEG as a static WASM library if not already built
if [ ! -f "$OPENJPEG_DIR/build/bin/libopenjp2.a" ]; then
  echo "==> Cloning OpenJPEG v2.5.2..."
  if [ ! -d "$OPENJPEG_DIR/.git" ]; then
    git clone --depth 1 --branch v2.5.2 https://github.com/uclouvain/openjpeg.git "$OPENJPEG_DIR"
  fi

  echo "==> Building OpenJPEG to WASM..."
  mkdir -p "$OPENJPEG_DIR/build"
  cd "$OPENJPEG_DIR/build"
  emcmake cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DBUILD_TESTING=OFF \
    -DBUILD_CODEC=OFF \
    -DBUILD_JPIP=OFF
  make -j"$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)" openjp2
  cd "$REPO_ROOT"
else
  echo "==> OpenJPEG WASM build found, skipping."
fi

OPENJPEG_INCLUDE="$OPENJPEG_DIR/src/lib/openjp2"
OPENJPEG_LIB="$OPENJPEG_DIR/build/bin/libopenjp2.a"

mkdir -p "$OUT_DIR"

echo "==> Compiling convert.c to WASM..."
emcc "$SCRIPT_DIR/convert.c" \
  -I "$OPENJPEG_INCLUDE" \
  "$OPENJPEG_LIB" \
  -O3 \
  -s MODULARIZE=1 \
  -s ENVIRONMENT=node \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s EXPORTED_FUNCTIONS='["_decode_jp2","_free_buffer","_malloc","_free"]' \
  -s EXPORTED_RUNTIME_METHODS='["getValue","HEAPU8"]' \
  -o "$OUT_DIR/openjp2.js"

echo "==> Done. Generated:"
ls -lh "$OUT_DIR"/openjp2.*
