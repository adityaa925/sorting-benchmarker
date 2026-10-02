# Complexity Analysis: Theoretical vs Empirical

## Overview

This document compares the theoretical time and space complexity of each sorting algorithm against empirical benchmark results.

## Algorithms Analyzed

| Algorithm | Best | Average | Worst | Space | Stable | In-Place |
|-----------|------|---------|-------|-------|--------|----------|
| Bubble Sort | O(N) | O(N²) | O(N²) | O(1) | Yes | Yes |
| Insertion Sort | O(N) | O(N²) | O(N²) | O(1) | Yes | Yes |
| Merge Sort | O(N log N) | O(N log N) | O(N log N) | O(N) | Yes | No |
| Quick Sort | O(N log N) | O(N log N) | O(N²) | O(log N) | No | Yes |
| Quick Sort (3-Way) | O(N log N) | O(N log N) | O(N²) | O(log N) | No | Yes |
| std::sort (Introsort) | O(N log N) | O(N log N) | O(N log N) | O(log N) | No | Yes |
| std::stable_sort | O(N log N) | O(N log N) | O(N log² N) | O(N) | Yes | No |

## Key Theoretical Insights

### Bubble Sort
- **Early-exit optimization**: Best case O(N) when already sorted
- **Practical use**: Only for tiny or nearly-sorted arrays (< 50 elements)
- **Cache behavior**: Poor - sequential access but many passes

### Insertion Sort
- **Best case**: O(N) for already sorted
- **Nearly sorted**: Excellent performance - O(N + k) where k is inversions
- **Small arrays**: Often faster than O(N log N) algorithms due to low overhead
- **Used in**: std::sort for small partitions (typically < 16 elements)

### Merge Sort
- **Guaranteed**: O(N log N) in all cases
- **Stable**: Preserves relative order of equal elements
- **Space**: Requires O(N) auxiliary space
- **Cache**: Good sequential access patterns
- **Parallelizable**: Natural divide-and-conquer structure

### Quick Sort
- **Average**: O(N log N) with good pivot selection
- **Worst case**: O(N²) - mitigated by median-of-three pivot
- **Hoare partition**: Fewer swaps than Lomuto
- **3-Way partition**: Handles duplicates efficiently (Dutch National Flag)
- **In-place**: Only O(log N) stack space

### Introsort (std::sort)
- **Hybrid**: QuickSort + HeapSort + InsertionSort
- **Worst case**: O(N log N) guaranteed via HeapSort fallback
- **Practical**: Best general-purpose sorting algorithm

## Expected Empirical Patterns

### Random Data
- Quick Sort ≈ std::sort > Merge Sort >> Insertion Sort > Bubble Sort
- Quick Sort 3-Way similar to Quick Sort

### Sorted Data
- Insertion Sort > Bubble Sort (early exit) > std::sort ≈ Quick Sort > Merge Sort
- Insertion Sort approaches O(N)

### Reverse Sorted
- std::sort ≈ Quick Sort (median-of-three) > Merge Sort
- Bubble Sort / Insertion Sort: O(N²) - worst case

### Nearly Sorted (5% disorder)
- Insertion Sort >> std::sort > Quick Sort > Merge Sort
- Insertion Sort: O(N + k) where k is small

### High Duplicates
- Quick Sort 3-Way >> std::sort > Quick Sort > Merge Sort
- 3-Way partition groups equals together

## Cache Performance Considerations

1. **Sequential Access**: Merge Sort and Insertion Sort have good locality
2. **Random Access**: Quick Sort partition step jumps around
3. **Auxiliary Space**: Merge Sort's buffer can cause cache pressure for large N
4. **Stack Depth**: Quick Sort recursion depth affects cache

## Scaling Behavior

| N | Bubble/Insertion | Merge/Quick/std::sort |
|---|------------------|----------------------|
| 100 | Fast | Fast |
| 1,000 | Measurable | Fast |
| 10,000 | Slow | Fast |
| 100,000 | Very Slow | Fast |
| 1,000,000 | Impractical | Fast |

## Measurement Methodology

- **Timer**: std::chrono::high_resolution_clock (nanosecond precision)
- **Warmup**: 3 runs (cache warming, JIT, branch prediction)
- **Measurement**: 5 runs, reporting mean/median/stddev
- **Metrics**: Deterministic comparison/move counts via TrackedElement
- **Coefficient of Variation**: Measures consistency (target < 5%)

## Interpreting Results

- **Logarithmic scale**: Reveals polynomial vs linearithmic scaling
- **CV%**: High variance indicates system noise or algorithmic instability
- **Comparisons vs Time**: Comparison count should match theoretical; time includes cache/memory effects
- **Speedup vs std::sort**: std::sort is the practical baseline

## Known Anomalies to Watch

1. **Small N**: Insertion Sort may beat O(N log N) algorithms due to overhead
2. **Cache effects**: Sudden performance drops at cache size boundaries
3. **Branch prediction**: Nearly-sorted data benefits from predictable branches
4. **Allocator**: std::vector allocation in Merge Sort adds overhead
5. **Denormalized floats**: Can slow down floating-point comparisons