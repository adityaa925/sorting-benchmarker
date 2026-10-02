# Sorting Algorithms Benchmarking Tool

A high-performance C++20 benchmarking suite that evaluates **Bubble Sort**, **Insertion Sort**, **Merge Sort**, **Quick Sort**, and **Quick Sort (3-Way)** across diverse data distributions and input scales, measuring execution time, comparison counts, swap counts, and cache performance against standard library baselines (`std::sort`, `std::stable_sort`).

## Features

- **Statistical Rigor**: Multi-run averaging (Mean, Median, Standard Deviation) with cache warmup runs
- **Hardware Metrics**: Nanosecond-precision timing + deterministic operational metrics (exact comparisons/swaps)
- **5 Data Distributions**: Random, Sorted, Reverse Sorted, Nearly Sorted (5% disorder), High Duplicates
- **Auto-Throttling**: Skips O(N²) algorithms past safe thresholds (configurable)
- **Rich Output**: Console tables, CSV, JSON, and interactive HTML/Chart.js visualization
- **Correctness Verification**: Built-in unit tests validating sorting invariants

## Building

Requires CMake 3.20+ and a C++20 compiler (GCC 10+, Clang 12+, MSVC 19.28+).

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### Build Options

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

## Usage

```bash
# Run with defaults
./sorting_benchmarker

# Custom sizes
./sorting_benchmarker --sizes 100,1000,10000,100000

# Specific distributions
./sorting_benchmarker --distributions random,nearly,duplicates

# Specific algorithms
./sorting_benchmarker --algorithms bubble,insertion,merge,quick,std

# More measurement runs
./sorting_benchmarker --warmup 5 --runs 10

# Disable O(N²) skip
./sorting_benchmarker --no-skip-n2

# Custom output files
./sorting_benchmarker --csv my_results.csv --json my_results.json --html my_report.html

# Random seed
./sorting_benchmarker --seed 12345
```

## Output Files

| File | Description |
|------|-------------|
| `benchmark_results.csv` | Raw data for analysis (pandas, R, Excel) |
| `benchmark_results.json` | Structured data for web visualization |
| `report.html` | Interactive Chart.js dashboard with log-scale toggle |

## Example Output

```
========================================================================================================================
                                     SORTING ALGORITHM BENCHMARK RESULTS
========================================================================================================================

Algorithm                Distribution       Size      Mean (ns)    Median (ns)   StdDev (ns)   CV%     Comparisons    Moves   Status
--------------------------------------------------------------------------------------------------------------------------------------------------------
Bubble Sort              Random             100       1.23M        1.19M         0.12M         9.8%    4.95K          2.48K   OK
Insertion Sort           Random             100       0.45M        0.43M         0.04M         8.2%    2.48K          1.24K   OK
Merge Sort               Random             100       0.18M        0.17M         0.02M         10.5%   521            1.02K   OK
Quick Sort               Random             100       0.12M        0.11M         0.01M         8.7%    612            0.38K   OK
Quick Sort (3-Way)       Random             100       0.14M        0.13M         0.02M         11.2%   589            0.41K   OK
std::sort (Introsort)    Random             100       0.09M        0.08M         0.01M         9.1%    534            0.32K   OK
std::stable_sort         Random             100       0.15M        0.14M         0.02M         10.8%   687            1.23K   OK
...
```

## Algorithm Implementations

| Algorithm | Key Optimizations |
|-----------|-------------------|
| **Bubble Sort** | Early-exit on no swaps; shrinks inner loop bound |
| **Insertion Sort** | Shift-based (not swap-based); Binary insertion variant available |
| **Merge Sort** | Single pre-allocated buffer; top-down recursive |
| **Quick Sort** | Median-of-three pivot; Hoare partition; InsertionSort for small partitions (≤16) |
| **Quick Sort 3-Way** | Dutch National Flag partition; handles duplicates optimally |
| **std::sort** | Introsort (QuickSort + HeapSort fallback + InsertionSort) |

## Benchmark Configuration

### Default Input Sizes
- Micro: 100, 500, 1,000
- Medium: 5,000, 10,000, 25,000
- Large: 50,000, 100,000, 250,000, 1,000,000

### Distributions
1. **Random**: Uniform distribution across full range
2. **Sorted**: Strictly ascending
3. **Reverse Sorted**: Strictly descending
4. **Nearly Sorted**: 5% elements swapped with neighbors
5. **High Duplicates**: Only 5 unique values

## Visualization

Open `report.html` in any modern browser for interactive charts:
- **Time vs N**: Logarithmic scale reveals O(N²) vs O(N log N) curves
- **Comparisons vs N**: Theoretical vs actual comparison counts
- **Moves vs N**: Data movement patterns
- **CV%**: Measurement consistency across runs

## Correctness Testing

The tool runs automated correctness tests before benchmarking:
- Empty/single element arrays
- Already sorted / reverse sorted
- Random permutations
- Duplicate-heavy arrays
- Stability verification (Merge Sort, std::stable_sort)
- Metric tracking validation

## Requirements

- CMake ≥ 3.20
- C++20 compiler
- No external dependencies (Chart.js loaded via CDN in HTML)

## License

MIT License - See LICENSE file for details.

## References

- Cormen et al., *Introduction to Algorithms* (CLRS)
- Musser, *Introspective Sorting and Selection Algorithms* (1997)
- Bentley & McIlroy, *Engineering a Sort Function* (1993)
- C++ Standard Library Reference