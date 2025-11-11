# Parallel Replace_if Benchmark

This C++ program benchmarks different ways of replacing odd numbers in a large vector (10 million integers) with a new value (`-1`) using various execution policies and a custom multithreaded algorithm.

---

## Features

- Uses `std::replace_if` with different execution policies: `seq`, `par`, `unseq`, `par_unseq`  
- Measures execution time for each policy  
- Implements a custom parallel algorithm with configurable number of threads  
- Outputs results to `output.txt`  

---

## Requirements

- C++17 or higher (`<execution>` support)  
- Standard C++ compiler (e.g., `g++`)  
