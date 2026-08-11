# Building IDR Analysis C++ Project

## System Requirements

### Mandatory
- **C++ Compiler**: GCC 7+ or Clang 5+ (with C++17 support)
- **CMake**: 3.10 or later
- **Eigen3**: 3.3 or later

### Build Tools
- **Make**: GNU Make or equivalent
- **Git**: For cloning dependencies (optional)

### Supported Platforms
- Linux (Ubuntu, Debian, CentOS, Fedora)
- macOS (10.14+)
- Windows (with MinGW or MSVC)

## Installation by Platform

### Ubuntu/Debian

1. **Install build tools and dependencies:**
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libeigen3-dev
```

2. **Verify installation:**
```bash
g++ --version
cmake --version
pkg-config --modversion eigen3
```

### macOS

1. **Install Homebrew (if not already installed):**
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

2. **Install dependencies:**
```bash
brew install cmake eigen
```

3. **Verify installation:**
```bash
clang++ --version
cmake --version
pkg-config --modversion eigen3
```

### CentOS/RHEL

```bash
sudo yum groupinstall "Development Tools"
sudo yum install cmake eigen3-devel
```

### Fedora

```bash
sudo dnf install cmake eigen3-devel
```

## Building the Project

### Quick Build (Recommended)

Use the provided build script:

```bash
cd /run/media/liveuser/SANDI_LARGE/IDR_Project
./build.sh
```

This will:
1. Create `build/` directory
2. Run CMake configuration
3. Compile the project
4. Place executables in `build/bin/`

### Manual Build

#### Step 1: Navigate to project directory
```bash
cd /run/media/liveuser/SANDI_LARGE/IDR_Project
```

#### Step 2: Create build directory
```bash
mkdir -p build
cd build
```

#### Step 3: Run CMake
```bash
cmake ..
```

For release build with optimizations:
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

#### Step 4: Compile
```bash
make
```

Or with multiple parallel jobs:
```bash
make -j4
```

#### Step 5: Verify build
```bash
ls -la bin/
```

Should show `idr_analysis` and optionally `run_tests`

## Build Options

### Optimization Levels

**Release (Optimized):**
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

**Debug (With symbols for debugging):**
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

**Release with Debug Info:**
```bash
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
```

### Custom Compiler

Specify a different C++ compiler:

```bash
cmake -DCMAKE_CXX_COMPILER=clang++ ..
```

Or:
```bash
cmake -DCMAKE_CXX_COMPILER=/usr/bin/g++ ..
```

### Custom Eigen Location

If Eigen3 is installed in a non-standard location:

```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/eigen3 ..
```

### Enable Verbose Output

```bash
make VERBOSE=1
```

## Testing

### Run Tests

```bash
./build/bin/run_tests
```

### Build Tests Only

```bash
cd build
cmake ..
make run_tests
```

## Clean Build

### Remove build artifacts

```bash
rm -rf build
```

### Full clean rebuild

```bash
rm -rf build
./build.sh
```

## Troubleshooting

### CMake: "Could not find Eigen3"

**Solution 1: Install Eigen3**
```bash
sudo apt-get install libeigen3-dev  # Debian/Ubuntu
brew install eigen                   # macOS
```

**Solution 2: Specify Eigen path**
```bash
cmake -DCMAKE_PREFIX_PATH=/usr/local/include/eigen3 ..
```

**Solution 3: Use pkg-config path**
```bash
export PKG_CONFIG_PATH=/usr/lib/pkgconfig:$PKG_CONFIG_PATH
cmake ..
```

### Error: "No C++ compiler found"

**Ubuntu/Debian:**
```bash
sudo apt-get install build-essential
```

**macOS:**
```bash
xcode-select --install
```

### C++ standard version error

Ensure C++17 support. Update compiler:

**GCC:**
```bash
sudo apt-get install g++-9  # Or higher version
cmake -DCMAKE_CXX_COMPILER=g++-9 ..
```

**Clang:**
```bash
sudo apt-get install clang-10  # Or higher version
cmake -DCMAKE_CXX_COMPILER=clang++-10 ..
```

### "make: command not found"

Install build tools:
```bash
sudo apt-get install build-essential  # Debian/Ubuntu
brew install make                      # macOS
```

### Build succeeds but executable doesn't run

Verify Eigen library can be found:
```bash
ldd ./build/bin/idr_analysis  # Linux
otool -L ./build/bin/idr_analysis  # macOS
```

## Advanced Build Configuration

### Enable Link-Time Optimization (LTO)

```bash
cmake -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON ..
make
```

### Build with AddressSanitizer (detect memory errors)

```bash
cmake -DCMAKE_CXX_FLAGS="-fsanitize=address -g" ..
make
```

### Build with coverage analysis

```bash
cmake -DCMAKE_CXX_FLAGS="--coverage" ..
make
```

## Installation

After building, optionally install to system:

```bash
cd build
sudo make install
```

This installs to `/usr/local/bin/` by default.

Custom install location:
```bash
cmake -DCMAKE_INSTALL_PREFIX=$HOME/local ..
make install
```

## Using in Other Projects

### Include as subdirectory

In your `CMakeLists.txt`:
```cmake
add_subdirectory(idr_project)
target_link_libraries(your_project idr_analysis)
```

### Use installed version

After `sudo make install`:
```cmake
find_package(IDR_Analysis REQUIRED)
target_link_libraries(your_project IDR::idr_analysis)
```

## Continuous Integration

### GitHub Actions Example

```yaml
name: Build and Test
on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install cmake libeigen3-dev
      - name: Build
        run: ./build.sh
      - name: Test
        run: ./build/bin/run_tests
```

## Performance Tuning

### Compiler Flags for Performance

```bash
cmake -DCMAKE_CXX_FLAGS="-O3 -march=native -flto" ..
make
```

### Using Intel compiler (if available)

```bash
cmake -DCMAKE_CXX_COMPILER=icpc ..
make
```

## Build Documentation

### Generate with Doxygen

```bash
sudo apt-get install doxygen
doxygen
```

Opens documentation in `docs/html/index.html`

## Common Build Errors

| Error | Solution |
|-------|----------|
| `CMake Error: Could not find Eigen3` | Install libeigen3-dev or set CMAKE_PREFIX_PATH |
| `error: 'feature' is not a member of 'std'` | Update compiler for C++17 support |
| `undefined reference to 'Eigen::...'` | Link with Eigen3::Eigen in CMakeLists.txt |
| `make: *** No targets specified` | Run `cmake ..` first, not just `make` |

## Getting Help

1. Check CMake output for error messages
2. Review `CMakeLists.txt` for configuration
3. Consult Eigen documentation: https://eigen.tuxfamily.org/
4. Check compiler documentation for compatibility

---

**For issues, check:**
- Compiler version: `g++ --version` or `clang++ --version`
- CMake version: `cmake --version`
- Eigen version: `pkg-config --modversion eigen3`
