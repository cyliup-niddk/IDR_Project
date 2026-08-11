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
cd IDR_Project
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
./build/bin/idr_analysis peak_file1 peak_file2
```

### 4. Run Tests

```bash
./build/bin/run_tests
```
