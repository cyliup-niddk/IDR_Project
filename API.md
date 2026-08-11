# IDR Analysis - C++ API Reference

Complete API documentation for the IDR Analysis C++ library.

## Table of Contents

1. [Data Structures](#data-structures)
2. [Utility Functions](#utility-functions)
3. [Optimization Functions](#optimization-functions)
4. [Constants](#constants)
5. [Example Usage](#example-usage)

---

## Data Structures

### Theta - Model Parameters

```cpp
struct Theta {
    double mu;      // Signal mean parameter
    double sigma;   // Standard deviation parameter
    double rho;     // Correlation coefficient
    double p;       // Mixture proportion (signal probability)
    
    // Method to convert to Eigen vector
    Eigen::Vector4d toVector() const;
};
```

**Purpose:** Represents the four parameters of the IDR statistical model.

**Parameters:**
- `mu` ∈ [0.0, 10.0] - Average signal value
- `sigma` ∈ [0.01, 10.0] - Standard deviation
- `rho` ∈ [0.0, 0.99] - Correlation between replicates
- `p` ∈ [0.01, 0.99] - Probability of signal component

**Example:**
```cpp
Theta params = {1.0, 0.5, 0.6, 0.7};
std::cout << "Mu: " << params.mu << std::endl;
```

### Peak - Genomic Peak Data

```cpp
struct Peak {
    std::string chrom;  // Chromosome identifier
    int start;          // Peak start position (bp)
    int stop;           // Peak end position (bp)
    double signal;      // Signal value (from column 7 of narrowPeak)
};
```

**Purpose:** Represents a genomic peak from a ChIP-seq or similar experiment.

**Fields:**
- `chrom` - Chromosome (e.g., "chr1", "chrX")
- `start` - Genomic start coordinate
- `stop` - Genomic end coordinate
- `signal` - Peak signal value used for ranking

**Example:**
```cpp
Peak peak = {"chr1", 1000, 1500, 8.5};
std::cout << peak.chrom << ":" << peak.start << "-" << peak.stop << std::endl;
```

---

## Utility Functions

### File I/O

#### load_narrow_peak()

```cpp
std::vector<Peak> load_narrow_peak(const std::string& filename);
```

**Purpose:** Load peaks from a narrowPeak format file.

**Parameters:**
- `filename` - Path to narrowPeak file

**Returns:** Vector of Peak objects

**Format:** Tab-separated values:
```
chrom   start   stop    name    score   strand   signalValue
chr1    1000    1500    peak1   100     +        5.5
```

**Requirements:**
- At least 7 columns required
- Column 1: chromosome
- Columns 2-3: coordinates
- Column 7: signal value
- Comments (lines starting with #) are ignored
- Empty lines are skipped

**Example:**
```cpp
std::vector<Peak> peaks = load_narrow_peak("replicate1.narrowPeak");
std::cout << "Loaded " << peaks.size() << " peaks" << std::endl;
```

### Mathematical Functions

#### NormalCDFInverse()

```cpp
double NormalCDFInverse(double p);
```

**Purpose:** Compute the inverse of the standard normal CDF (quantile function).

**Parameters:**
- `p` - Probability value ∈ (0, 1)

**Returns:** Quantile value (inverse CDF)

**Domain:**
- p ≤ 0 → returns -5.0
- p ≥ 1 → returns 5.0
- 0 < p < 1 → rational approximation

**Precision:** ~4 decimal places

**Example:**
```cpp
double q = NormalCDFInverse(0.05);  // ~-1.645
double med = NormalCDFInverse(0.5); // ~0.0
double q95 = NormalCDFInverse(0.95);// ~1.645
```

### Vector Operations

#### compute_pseudo_values()

```cpp
Eigen::VectorXd compute_pseudo_values(
    const Eigen::VectorXd& ranks,
    double mu,
    double sigma,
    double p
);
```

**Purpose:** Transform rank values to pseudo-values in normal space.

**Parameters:**
- `ranks` - Vector of rank values
- `mu` - Signal mean parameter
- `sigma` - Std dev parameter
- `p` - Mixture proportion parameter

**Returns:** Vector of pseudo-values (same size as input)

**Algorithm:**
1. For each rank r in [0, n-1]:
2. Compute percentile: q = (r + 1) / (n + 1)
3. Compute pseudo-value: z = Φ⁻¹(q)

**Note:** Parameters don't affect transformation (used for consistency)

**Example:**
```cpp
Eigen::VectorXd ranks = Eigen::VectorXd::LinSpaced(100, 0, 99);
Eigen::VectorXd z = compute_pseudo_values(ranks, 1.0, 0.5, 0.5);
```

#### calc_post_membership_prbs()

```cpp
Eigen::VectorXd calc_post_membership_prbs(
    const Theta& t,
    const Eigen::VectorXd& z1,
    const Eigen::VectorXd& z2
);
```

**Purpose:** Calculate posterior membership probabilities (E-step of EM).

**Parameters:**
- `t` - Model parameters (Theta)
- `z1` - Pseudo-values from replicate 1
- `z2` - Pseudo-values from replicate 2

**Returns:** Vector of probabilities ∈ [0, 1] (same size as input)

**Formula:**
```
P(signal | z1, z2, θ) = (p * f_signal) / (p * f_signal + (1-p) * f_noise)
```

Where:
- `f_signal` = bivariate normal density with parameters θ
- `f_noise` = independent normal noise density

**Interpretation:**
- High values (close to 1): Peak likely reproducible
- Low values (close to 0): Peak likely noise

**Example:**
```cpp
Theta theta = {1.0, 0.5, 0.6, 0.7};
Eigen::VectorXd z1(100), z2(100);
// ... populate z1, z2 ...
Eigen::VectorXd probs = calc_post_membership_prbs(theta, z1, z2);
```

---

## Optimization Functions

### EM_step()

```cpp
Theta EM_step(
    const Eigen::VectorXd& z1,
    const Eigen::VectorXd& z2,
    const Theta& current
);
```

**Purpose:** Perform one iteration of the EM algorithm.

**Parameters:**
- `z1` - Pseudo-values from replicate 1
- `z2` - Pseudo-values from replicate 2
- `current` - Current model parameters

**Returns:** Updated model parameters

**Algorithm:**
1. **E-step:** Calculate posterior probabilities using `calc_post_membership_prbs()`
2. **M-step:** Update parameters based on posteriors
3. **Constraint:** Clamp all parameters to valid ranges

**Parameter Updates (M-step):**
- `mu` = (Σ eᵢ(z1ᵢ + z2ᵢ)) / (2 * Σ eᵢ)
- `sigma` = √((Σ eᵢ((z1ᵢ - μ)² + (z2ᵢ - μ)²)) / (2 * Σ eᵢ))
- `rho` = (2 * Σ eᵢ(z1ᵢ - μ)(z2ᵢ - μ)) / (Σ eᵢ((z1ᵢ - μ)² + (z2ᵢ - μ)²))
- `p` = (Σ eᵢ) / n

Where eᵢ = posterior probability for observation i

**Example:**
```cpp
Theta current = {1.0, 1.0, 0.5, 0.5};
Eigen::VectorXd z1(100), z2(100);
// ... populate z1, z2 ...

Theta updated = EM_step(z1, z2, current);
std::cout << "Mu updated to: " << updated.mu << std::endl;
```

### run_idr()

```cpp
Theta run_idr(
    const Eigen::VectorXd& r1,
    const Eigen::VectorXd& r2,
    Theta init
);
```

**Purpose:** Run the complete IDR optimization loop using EM algorithm.

**Parameters:**
- `r1` - Rank vector from replicate 1
- `r2` - Rank vector from replicate 2
- `init` - Initial model parameters

**Returns:** Optimized model parameters

**Algorithm:**
1. Initialize θ = init
2. Repeat up to 100 iterations:
   - Compute pseudo-values: z1, z2 from ranks
   - Perform EM step: θ_new = EM_step(z1, z2, θ)
   - Check convergence: ||θ_new - θ||₁ < 1e-6
   - If converged, break
3. Return final θ

**Convergence:** Uses L1 norm of parameter vector difference

**Example:**
```cpp
Eigen::VectorXd r1 = Eigen::VectorXd::LinSpaced(100, 0, 99);
Eigen::VectorXd r2 = Eigen::VectorXd::LinSpaced(100, 0, 99);

Theta initial = {1.0, 1.0, 0.5, 0.5};
Theta result = run_idr(r1, r2, initial);

std::cout << "Final mu: " << result.mu << std::endl;
std::cout << "Final p: " << result.p << std::endl;
```

---

## Constants

All defined in `include/utility.hpp`:

```cpp
const double MIN_MU = 0.0;
const double MAX_MU = 10.0;

const double MIN_SIGMA = 0.01;
const double MAX_SIGMA = 10.0;

const double MIN_RHO = 0.0;
const double MAX_RHO = 0.99;

const double MIN_MIX_PARAM = 0.01;
const double MAX_MIX_PARAM = 0.99;
```

These constraints ensure parameters stay in valid ranges after EM updates.

---

## Example Usage

### Complete Pipeline

```cpp
#include <iostream>
#include <vector>
#include "utility.hpp"
#include "optimization.hpp"

int main() {
    // 1. Load peak files
    std::vector<Peak> peaks1 = load_narrow_peak("replicate1.narrowPeak");
    std::vector<Peak> peaks2 = load_narrow_peak("replicate2.narrowPeak");
    
    std::cout << "Loaded " << peaks1.size() << " and " 
              << peaks2.size() << " peaks" << std::endl;
    
    // 2. Find overlapping peaks and rank them
    std::vector<double> s1_vals, s2_vals;
    for (const auto& peak_a : peaks1) {
        for (const auto& peak_b : peaks2) {
            if (peak_a.chrom == peak_b.chrom) {
                int overlap_start = std::max(peak_a.start, peak_b.start);
                int overlap_end = std::min(peak_a.stop, peak_b.stop);
                
                if (overlap_start < overlap_end) {
                    s1_vals.push_back(peak_a.signal);
                    s2_vals.push_back(peak_b.signal);
                    break;
                }
            }
        }
    }
    
    if (s1_vals.size() < 20) {
        std::cerr << "Error: Fewer than 20 overlapping peaks" << std::endl;
        return 1;
    }
    
    // 3. Convert to ranks
    auto get_ranks = [](const std::vector<double>& scores) {
        Eigen::VectorXd ranks(scores.size());
        std::vector<int> indices(scores.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::sort(indices.begin(), indices.end(),
                  [&](int i, int j) { return scores[i] < scores[j]; });
        for (int i = 0; i < indices.size(); ++i) {
            ranks[indices[i]] = i;
        }
        return ranks;
    };
    
    Eigen::VectorXd r1 = get_ranks(s1_vals);
    Eigen::VectorXd r2 = get_ranks(s2_vals);
    
    // 4. Run IDR optimization
    Theta initial = {1.0, 1.0, 0.5, 0.5};
    Theta result = run_idr(r1, r2, initial);
    
    // 5. Output results
    std::cout << "\n=== Final IDR Model Parameters ===" << std::endl;
    std::cout << "Mu (Signal Mean):    " << result.mu << std::endl;
    std::cout << "Sigma (Std Dev):     " << result.sigma << std::endl;
    std::cout << "Rho (Correlation):   " << result.rho << std::endl;
    std::cout << "P (Mix Proportion):  " << result.p << std::endl;
    
    return 0;
}
```

### Using Eigen Vectors

```cpp
#include <Eigen/Dense>
#include "utility.hpp"

int main() {
    // Create vectors
    Eigen::VectorXd ranks = Eigen::VectorXd::LinSpaced(50, 0, 49);
    
    // Compute pseudo-values
    Theta params = {1.5, 0.8, 0.6, 0.7};
    Eigen::VectorXd z = compute_pseudo_values(ranks, 
                                               params.mu, 
                                               params.sigma, 
                                               params.p);
    
    // Compute probabilities
    Eigen::VectorXd z1 = z;
    Eigen::VectorXd z2 = z + Eigen::VectorXd::Random(50) * 0.1;
    
    Eigen::VectorXd probs = calc_post_membership_prbs(params, z1, z2);
    
    std::cout << "Probabilities (first 5): ";
    for (int i = 0; i < 5; ++i) {
        std::cout << probs[i] << " ";
    }
    std::cout << std::endl;
    
    return 0;
}
```

---

## Error Handling

Most functions perform minimal error checking. Ensure:
1. Input vectors have compatible sizes
2. narrowPeak file is properly formatted
3. At least 20 overlapping peaks exist
4. Rank vectors contain valid values (0 to n-1)

---

## Performance Considerations

- **Vector size O(n):** All operations are linear in data size
- **Eigen optimization:** Automatic SIMD acceleration
- **Memory:** Linear in number of peaks
- **Time per EM iteration:** ~O(n) operations

---

## Thread Safety

The library is NOT thread-safe. Use synchronization if calling from multiple threads.

---

**For more information, see:**
- README.md - Project overview
- BUILDING.md - Build instructions
- Source code in src/ directory

