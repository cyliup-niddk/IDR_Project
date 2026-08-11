# IDR Analysis - C++ Examples

## Sample Data

This directory contains sample narrowPeak files for testing the IDR analysis program.

### Files

- **sample_replicate1.narrowPeak** - Peak data from replicate 1 (25 peaks)
- **sample_replicate2.narrowPeak** - Peak data from replicate 2 (25 peaks)

### Data Format

narrowPeak format (tab-separated):

```
chrom    start    stop     name     score   strand   signalValue
chr1     1000     1500     peak1    100     +        8.5
chr1     2000     2300     peak2    95      +        7.8
...
```

**Columns:**
1. Chromosome
2. Peak start position
3. Peak end position
4. Peak name
5. Score
6. Strand
7. **Signal Value** (used for ranking)

### Running Tests

From the project root directory:

```bash
# Build the project
./build.sh

# Run with sample data
./build/bin/idr_analysis examples/sample_replicate1.narrowPeak examples/sample_replicate2.narrowPeak
```

### Expected Output

```
Loading peaks from: examples/sample_replicate1.narrowPeak
Loaded 25 peaks

Loading peaks from: examples/sample_replicate2.narrowPeak
Loaded 25 peaks

Processing 25 overlapping peaks...

--- Final IDR Model Parameters ---
Mu (Signal Mean):      0.000314
Sigma (Std Dev):       0.888092
Rho (Correlation):     0.990000
P (Mix Proportion):    0.990000
```

### Using Your Own Data

Replace the sample files with your own narrowPeak files:

```bash
./build/bin/idr_analysis your_replicate1.narrowPeak your_replicate2.narrowPeak
```

**Requirements:**
- At least 20 overlapping peaks between replicates
- Valid narrowPeak format
- Column 7 contains numeric signal values

### Data Characteristics

The sample data contains:
- 25 peaks per replicate
- Distributed across 5 chromosomes (chr1-chr5)
- Overlapping peaks between replicates
- Realistic signal values (4.6-8.5)

