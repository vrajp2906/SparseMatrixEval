
# Sparse Matrix Performance Evaluation

## Overview

This project investigates the performance of sparse matrix operations using C and C++ implementations, with and without SIMD (Single Instruction, Multiple Data) optimizations. Sparse matrices—matrices with mostly zero elements—enable specialized algorithms and storage formats that can significantly reduce memory usage and improve computational efficiency. The project evaluates three fundamental operations:

- **Addition**: Element-wise addition of two sparse matrices.
- **Multiplication**: Matrix multiplication, which can result in denser matrices.
- **Vector Multiplication**: Multiplying a sparse matrix by a vector.

The project report (`project_report.pdf`) details the methodology, experiments, and analysis of performance across different matrix sizes, cache levels, optimization flags, and SIMD usage.

---

## Directory Structure

```
project-root/
├── code/
│   ├── 2d_sparse.c
│   ├── mispredictions.c
│   ├── sparse_matrix.c
│   ├── simd.cpp
│   └── sparse.c
├── results/
├── project_report.pdf
└── README.md
```

---

## Code Descriptions

- **`2d_sparse.c`**  
  C implementation using a dense 1D array to simulate a 2D matrix. Supports creation of random sparse arrays, addition, multiplication, matrix-vector multiplication, and timing of column-sum operations.

- **`mispredictions.c`**  
  C implementation using a dense array for sparse matrices. Includes a random branch in matrix multiplication to analyze branch prediction effects on performance. Measures and reports timings for all three core operations.

- **`sparse_matrix.c`**  
  C implementation using an explicit sparse format (array of structs, each storing row, column, value). Focuses on generating unique random nonzero elements and printing them.

- **`simd.cpp`**  
  C++ implementation leveraging SIMD (via Agner Fog's Vector Class Library) for vectorized addition, multiplication, and matrix-vector multiplication. Designed for high performance on large matrices.

- **`sparse.c`**  
  C implementation similar to `mispredictions.c`, focusing on basic sparse matrix operations and performance measurement.

---

## How to Build and Run

### Requirements

- **C compiler**: GCC recommended for C files.
- **C++ compiler**: g++ (with support for SIMD and C++11 or newer) for `simd.cpp`.
- **SIMD library**: Agner Fog's Vector Class Library (VCL) or equivalent for `simd.cpp`.

### Compilation Examples

#### C Files

```
gcc -O3 -o 2d_sparse code/2d_sparse.c -lm
gcc -O3 -o mispredictions code/mispredictions.c -lm
gcc -O3 -o sparse_matrix code/sparse_matrix.c -lm
gcc -O3 -o sparse code/sparse.c -lm
```

#### C++ SIMD File

```
g++ -O3 -march=native -std=c++11 -o simd code/simd.cpp
```

## Methodology & Evaluation

- **Test System**: 10th Gen Intel i7-1065G7, 4 cores/8 threads, WSL on Windows, Linux tools (Valgrind, Perf).
- **Benchmarking**: Timed each operation (addition, multiplication, vector-matrix multiplication) across varying matrix sizes and sparsity levels.
- **Cache Analysis**: Used Cachegrind to analyze L1/L2/L3 cache usage and the impact of memory locality.
- **Branch Prediction**: Used Perf to measure branch misprediction rates, especially in the random-branch multiplication implementation.
- **SIMD Comparison**: Compared standard C implementations with SIMD-optimized C++ code for speedup and efficiency.

---

## Key Results

- **Compiler Optimization**:  
  - `-O2` and `-O3` flags yield significant speedups, especially for addition and multiplication.
  - SIMD provides up to 3–4x speedup for large matrix multiplication compared to scalar C code.

- **Cache Locality**:  
  - Operations are fastest when the matrix fits in L1 cache.
  - Performance degrades as matrices exceed L1, L2, and L3 cache sizes due to increased cache misses.

- **Branch Prediction**:  
  - Branch misprediction rates remain low (<0.1%), even with randomized branching in multiplication.

- **SIMD Performance**:  
  - SIMD implementations outperform scalar C, especially for large matrices and multiplication.
  - SIMD also reduces branch mispredictions and improves memory locality.
