# Windows Cross-Compilation for micro-logger-cpp

This directory contains Docker-based infrastructure for cross-compiling micro-logger-cpp from Linux/macOS to Windows.

## Quick Start

### Cross-Compile to Windows
```bash
./cross-compile-windows.sh
```

**Output:** `build-windows/micro_logger_sample.exe`

### Test with Wine (Linux emulation)
```bash
./test-windows-with-wine.sh
```

This runs the Windows executable using Wine without needing a Windows machine.

## Available Scripts

### `cross-compile-windows.sh`
Docker-based cross-compilation wrapper.

**Usage:**
```bash
./cross-compile-windows.sh [OPTIONS]

Options:
  --clean      Remove build-windows/ directory before compiling
  --rebuild    Force rebuild of Docker image
  --shell      Open interactive shell in Docker container (for debugging)
  --help       Show help message
```

**What it does:**
1. Checks Docker availability
2. Builds Docker image if needed (Ubuntu 22.04 + mingw-w64-posix)
3. Runs `build-windows.sh` inside container
4. Copies `micro_logger_sample.exe` to `build-windows/`

### `test-windows-with-wine.sh`
Run Windows executable using Wine.

**Usage:**
```bash
./test-windows-with-wine.sh [OPTIONS]

Options:
  --rebuild    Force rebuild of Wine Docker image
  --help       Show help message
```

**What it does:**
1. Builds Wine Docker image if needed (Ubuntu 22.04 + wine64)
2. Runs `micro_logger_sample.exe` with wine64
3. Verifies output correctness

### `build-windows.sh`
Low-level build script (runs inside Docker or on host).

**Usage:**
```bash
# Inside Docker container:
./build-windows.sh

# On host with mingw-w64 installed:
./build-windows.sh
```

**What it does:**
- Detects Docker vs host environment
- Prefers POSIX threading compiler variant
- Compiles with static linking flags
- Outputs to `build-windows/`

## Docker Images

### `Dockerfile.windows-cross`
Cross-compilation environment for Windows targets.

**Base:** Ubuntu 22.04

**Installed packages:**
- `mingw-w64` (base toolchain)
- `g++-mingw-w64-x86-64-posix` ⚠️ **CRITICAL: POSIX variant required**
- `cmake`, `make`, `git`

**Compilers:**
- `x86_64-w64-mingw32-gcc-posix`
- `x86_64-w64-mingw32-g++-posix`

### `Dockerfile.wine-test`
Testing environment for running Windows executables.

**Base:** Ubuntu 22.04

**Installed packages:**
- `wine64` (Windows emulator)

## Requirements

### For Docker-based builds (Recommended):
- Docker Desktop installed and running
- No other dependencies needed

### For native builds (Advanced):
- mingw-w64 with **POSIX threading model**
- CMake 3.10+
- Make

**Ubuntu/Debian:**
```bash
sudo apt-get install g++-mingw-w64-x86-64-posix cmake make
```

**macOS (Homebrew):**
```bash
brew install mingw-w64 cmake
# Note: Check threading model with: x86_64-w64-mingw32-g++ -v 2>&1 | grep thread
```

## Threading Model: CRITICAL REQUIREMENT

⚠️ **micro-logger-cpp uses `std::mutex`, which requires POSIX threading model.**

MinGW-w64 has **two threading models**:

1. **win32 threading** (default)
   - Uses native Windows threading APIs
   - ❌ Does NOT support C++11 threading (`std::mutex`, `std::thread`, etc.)
   - Will fail to compile micro-logger-cpp

2. **POSIX threading** (winpthreads)
   - Uses POSIX threads implemented on Windows
   - ✅ Full C++11 threading support
   - **Required for micro-logger-cpp**

### How to check threading model:
```bash
x86_64-w64-mingw32-g++ -v 2>&1 | grep -i thread
```

**Expected (correct):**
```
--enable-threads=posix
Thread model: posix
```

**Wrong (will fail):**
```
--enable-threads=win32
Thread model: win32
```

### Installing the correct variant:

**Ubuntu/Debian:**
```bash
# Install POSIX variant explicitly:
apt-get install g++-mingw-w64-x86-64-posix

# Compiler names:
x86_64-w64-mingw32-gcc-posix
x86_64-w64-mingw32-g++-posix
```

**macOS:**
```bash
brew install mingw-w64

# Check which threading model was installed:
x86_64-w64-mingw32-g++ -v 2>&1 | grep thread

# If win32, you may need to use update-alternatives or install a different variant
```

## Compilation Flags

The build uses these flags for Windows cross-compilation:

```bash
-std=c++20              # C++20 standard (required by micro-logger-cpp)
-Wall -Wextra           # Enable all warnings
-O2                     # Optimization level 2
-static-libgcc          # Statically link libgcc (no DLL dependency)
-static-libstdc++       # Statically link libstdc++ (no DLL dependency)
-static                 # Statically link everything (standalone .exe)
```

This produces a **fully standalone .exe** with no external dependencies.

## Testing

### Test with Wine (Quick validation)
```bash
./test-windows-with-wine.sh
```

**Expected output:**
```
2025-10-23 19:14:17.147 | INFO    | My Class | I'm 27 years old!
2025-10-23 19:14:17.149 | INFO    | My Class | Test 123.456 test
2025-10-23 19:14:17.150 | WARNING | My Class | This is a warning
```

### Test on real Windows (Production validation)
1. Copy `build-windows/micro_logger_sample.exe` to a Windows machine
2. Run: `micro_logger_sample.exe`
3. Verify colored console output

## Troubleshooting

### Error: `std::mutex has not been declared`
**Cause:** Compiler is using win32 threading model.

**Solution:** Use POSIX threading compiler variant:
```bash
# Check current threading model:
x86_64-w64-mingw32-g++ -v 2>&1 | grep thread

# If shows "win32", install posix variant:
apt-get install g++-mingw-w64-x86-64-posix
```

### Docker build fails
**Cause:** Docker not running or insufficient permissions.

**Solution:**
```bash
# Check Docker is running:
docker ps

# If permission denied:
sudo usermod -aG docker $USER
# Log out and back in
```

### Wine test fails with "wine: command not found"
**Cause:** Wine Docker image not built or corrupted.

**Solution:**
```bash
./test-windows-with-wine.sh --rebuild
```

## File Structure

```
cross-compile/
├── README.md                      # This file
├── Dockerfile.windows-cross       # Cross-compilation environment
├── Dockerfile.wine-test           # Wine testing environment
├── cross-compile-windows.sh       # Main build wrapper (Docker orchestration)
├── build-windows.sh               # Build script (runs inside Docker)
├── test-windows-with-wine.sh      # Wine testing wrapper
├── sample/
│   └── micro_logger_sample.cpp    # Sample code for testing
└── build-windows/                 # Output directory (created by build)
    └── micro_logger_sample.exe    # Windows executable
```

## How It Works

### Cross-Compilation Flow:
```
1. cross-compile-windows.sh (host)
   ↓
2. Docker builds/starts container (Ubuntu 22.04 + mingw-w64-posix)
   ↓
3. build-windows.sh (inside container)
   ↓
4. x86_64-w64-mingw32-g++-posix compiles sample
   ↓
5. Output: build-windows/micro_logger_sample.exe
```

### Wine Testing Flow:
```
1. test-windows-with-wine.sh (host)
   ↓
2. Docker builds/starts Wine container (Ubuntu 22.04 + wine64)
   ↓
3. wine64 micro_logger_sample.exe
   ↓
4. Verify output correctness
```

## Technical Details

For a complete technical deep-dive including:
- Investigation timeline
- Threading model explanation
- Stack Overflow references
- Error message decoding
- Complete file changes

See: [`../WINDOWS_CROSS_COMPILATION_NOTES.md`](../WINDOWS_CROSS_COMPILATION_NOTES.md)

## Next Steps

After successfully cross-compiling micro-logger-cpp, the same pattern will be applied to:
1. ipc-eventbus (uses `std::thread`, `std::mutex`, `std::condition_variable`)
2. ipc-sharedmemory (may use threading primitives)
3. octopus-ipc (uses all of the above)
4. Octopus Pedal Unified Binary (full Windows support)

All will require POSIX threading model.
