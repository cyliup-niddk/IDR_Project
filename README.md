# IDR (Irreproducible Discovery Rate) Analysis - C++ Project

A complete C++ implementation of IDR (Irreproducible Discovery Rate) analysis for analyzing the reproducibility of peaks between biological replicates.

## Project Overview

This project implements the IDR statistical model using:
- **C++17** programming language
- **Eigen3** linear algebra library
- **CMake** build system

The software performs EM (Expectation-Maximization) optimization on genomic peak data to determine reproducibility metrics between replicate experiments.

## Quick Start

### 1. Prerequisites

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libeigen3-dev
```

**macOS:**
```bash
brew install cmake eigen
```

### 2. Build the Project

```bash
cd /run/media/liveuser/SANDI_LARGE/IDR_Project
./build.sh
```

Or manually:
```bash
mkdir -p build && cd build
cmake ..
make
```

### 3. Run Analysis

```bash
./build/bin/idr_analysis examples/sample_replicate1.narrowPeak examples/sample_replicate2.narrowPeak
```

### 4. Run Tests

```bash
./build/bin/run_tests
```

## Project Structure

```
IDR_Project/
├── src/                          # Source files
│   ├── main.cpp                  # Main program
│   ├── optimization.cpp          # EM algorithm implementation
│   └── utility.cpp               # Utility functions
│
├── include/                      # Header files
│   ├── optimization.hpp          # EM optimization declarations
│   └── utility.hpp               # Utility function declarations
│
├── tests/                        # Test files
│   └── test_main.cpp             # Unit tests
│
├── examples/                     # Sample data
│   ├── sample_replicate1.narrowPeak
│   └── sample_replicate2.narrowPeak
│
├── build/                        # Build directory (created by build.sh)
│
├── CMakeLists.txt               # CMake configuration
├── build.sh                      # Build script
├── README.md                     # This file
├── BUILDING.md                  # Detailed build guide
├── API.md                        # API documentation
└── LICENSE                       # MIT License
```

## Features

✅ **Load genomic peaks** from narrowPeak format files
✅ **Find overlapping peaks** between two replicates
✅ **Convert to ranks** for statistical analysis
✅ **Compute inverse normal CDF** for pseudo-value transformation
✅ **EM optimization** with convergence detection
✅ **Parameter constraints** (mu, sigma, rho, p)
✅ **Comprehensive error handling**
✅ **Unit tests** included

## Input Format

### narrowPeak Format

Tab-separated columns:
```
chrom    start    stop    name    score   strand   signalValue   [pvalue]   [qvalue]
chr1     1000     1500    peak1   100     +        5.5           -1         -1
chr1     2000     2300    peak2   95      +        4.8           -1         -1
```

**Required columns:** 1-7 (signalValue is column 7)
**Example:** See `examples/sample_replicate*.narrowPeak`

## Command-Line Usage

### Basic Usage
```bash
./idr_analysis <replicate1.narrowPeak> <replicate2.narrowPeak>
```

### Example
```bash
./build/bin/idr_analysis examples/sample_replicate1.narrowPeak examples/sample_replicate2.narrowPeak
```

## Output

The program outputs final model parameters:

```
============================================================
--- Final IDR Model Parameters ---
Mu (Signal Mean):      0.000314
Sigma (Std Dev):       0.888092
Rho (Correlation):     0.990000
P (Mix Proportion):    0.990000
============================================================
```

### Parameters

- **Mu**: Average signal value in the bivariate normal signal component
- **Sigma**: Standard deviation of the signal component
- **Rho**: Correlation coefficient between replicates
- **P**: Mixture proportion (probability of signal vs. noise)

Higher P values indicate better reproducibility between replicates.

## Model Parameters

All parameters are constrained to valid ranges:

| Parameter | Min | Max | Description |
|-----------|-----|-----|-------------|
| Mu | 0.0 | 10.0 | Signal mean |
| Sigma | 0.01 | 10.0 | Standard deviation |
| Rho | 0.0 | 0.99 | Correlation |
| P | 0.01 | 0.99 | Mix proportion |

## Data Structures

### Theta (Model Parameters)
```cpp
struct Theta {
    double mu;      // Signal mean
    double sigma;   // Standard deviation
    double rho;     // Correlation
    double p;       // Mix proportion
};
```

### Peak (Genomic Peak)
```cpp
struct Peak {
    std::string chrom;  // Chromosome
    int start;          // Start position
    int stop;           // End position
    double signal;      // Signal value (column 7)
};
```

## Core Functions

### Utility Module (`include/utility.hpp`)

```cpp
// Compute inverse normal CDF (quantile function)
double NormalCDFInverse(double p);

// Convert ranks to pseudo-values
Eigen::VectorXd compute_pseudo_values(
    const Eigen::VectorXd& ranks,
    double mu, double sigma, double p
);

// Calculate posterior membership probabilities
Eigen::VectorXd calc_post_membership_prbs(
    const Theta& t,
    const Eigen::VectorXd& z1,
    const Eigen::VectorXd& z2
);
```

### Optimization Module (`include/optimization.hpp`)

```cpp
// Perform one EM step
Theta EM_step(
    const Eigen::VectorXd& z1,
    const Eigen::VectorXd& z2,
    const Theta& current
);

// Run complete IDR optimization
Theta run_idr(
    const Eigen::VectorXd& r1,
    const Eigen::VectorXd& r2,
    Theta init
);
```

## Building & Testing

### Build Options

**Release build (optimized):**
```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

**Debug build (with symbols):**
```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

### Running Tests

```bash
./build/bin/run_tests
```

Expected output:
```
============================================================
IDR Analysis - C++ Unit Tests
============================================================
Testing: Theta structure creation... ✓ PASSED
Testing: Theta to vector conversion... ✓ PASSED
Testing: Normal CDF inverse - boundary... ✓ PASSED
...
============================================================
Test Results: 6 passed
============================================================
```

## Performance

- **Small datasets** (< 1000 peaks): < 1 second
- **Medium datasets** (1000-10000 peaks): 1-5 seconds
- **Large datasets** (> 10000 peaks): Scales well, linear time

Memory usage is minimal, even for large datasets.

## Requirements

- **C++17** or later
- **CMake 3.10** or later
- **Eigen3** (3.3+) for linear algebra
- **Unix-like system** (Linux, macOS)

## Troubleshooting

### CMake not found
```bash
sudo apt-get install cmake  # Ubuntu/Debian
brew install cmake          # macOS
```

### Eigen3 not found
```bash
sudo apt-get install libeigen3-dev  # Ubuntu/Debian
brew install eigen                  # macOS
```

### Build failures
- Ensure C++17 compiler is available
- Check CMake version: `cmake --version`
- Verify Eigen installation: `pkg-config --modversion eigen3`

### Program crashes
- Check file paths are correct
- Ensure input files are in narrowPeak format
- Verify at least 20 overlapping peaks exist

## Documentation

- [BUILDING.md](BUILDING.md) - Detailed build instructions
- [API.md](API.md) - Complete API reference
- [LICENSE](LICENSE) - MIT License

## Performance Notes

- Compiling with `-O3` optimization flag for best performance
- Eigen provides SIMD optimizations automatically
- Multi-threaded operations can be enabled if Eigen is built with OpenMP

## Limitations

- Minimum 20 overlapping peaks required for analysis
- Input file format must be valid narrowPeak
- Parameters are normalized to data scale

## Citation

Based on IDR methodology:
> Qunhua Li, James B. Brown, Haiyan Huang, Peter J. Bickel (2011)
> "Measuring reproducibility of high-throughput experiments"
> Annals of Applied Statistics 5(3): 1752-1779

## License

MIT License - See LICENSE file for details

## Author

IDR Analysis C++ Project

---

**Project Location:** `/run/media/liveuser/SANDI_LARGE/IDR_Project`
**Status:** Complete and Ready to Use
**Last Updated:** August 11, 2024
