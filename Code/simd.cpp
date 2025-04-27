#include <iostream>
#include <vectorclass.h>
#include <cstdlib>
#include <ctime>
#include <chrono>

#define DATA_T double
#define DATA_PRINTF "%g"
#define RAND_EXPR ((double)rand() / RAND_MAX - 0.5)

// Function to create an array of given length with random values
DATA_T* create_array(uint64_t length) {
    DATA_T* array = (DATA_T*)malloc(length * sizeof(DATA_T));
    if (array == NULL) {
        return NULL;
    }
    for (uint64_t i = 0; i < length; i++) {
        array[i] = RAND_EXPR;
    }
    return array;
}

// Function to add two sparse matrices
void add_sparse_matrices(DATA_T* A, DATA_T* B, DATA_T* C, uint64_t width, uint64_t height) {
    for (uint64_t i = 0; i < height; ++i) {
        for (uint64_t j = 0; j < width; j += 4) {
            Vec4d vecA(A[i * width + j], A[i * width + j + 1], A[i * width + j + 2], A[i * width + j + 3]);
            Vec4d vecB(B[i * width + j], B[i * width + j + 1], B[i * width + j + 2], B[i * width + j + 3]);
            Vec4d vecC = vecA + vecB;
            vecC.store(&C[i * width + j]);
        }
    }
}

// Function to multiply two sparse matrices
void multiply_sparse_matrices(DATA_T* A, DATA_T* B, DATA_T* C, uint64_t width, uint64_t height) {
    for (uint64_t i = 0; i < height; ++i) {
        for (uint64_t j = 0; j < width; j += 4) {
            Vec4d sum(0.0);
            for (uint64_t k = 0; k < width; ++k) {
                Vec4d vecA(A[i * width + k]);
                Vec4d vecB(B[k * width + j]);
                sum += vecA * vecB;
            }
            sum.store(&C[i * width + j]);
        }
    }
}

// Function for vector-matrix multiplication
void vector_matrix_multiplication(DATA_T* mat, DATA_T* vec, DATA_T* result, uint64_t width, uint64_t height) {
    for (uint64_t i = 0; i < height; ++i) {
        Vec4d sum(0.0);
        for (uint64_t j = 0; j < width; j += 4) {
            Vec4d matVec(mat[i * width + j], mat[i * width + j + 1], mat[i * width + j + 2], mat[i * width + j + 3]);
            Vec4d vecVec(vec[j], vec[j + 1], vec[j + 2], vec[j + 3]);
            sum += matVec * vecVec;
        }
        result[i] = sum[0] + sum[1] + sum[2] + sum[3];
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Must give array size and height on command line.\n";
        return 1;
    }
    uint64_t width = atol(argv[1]);
    uint64_t height = atol(argv[2]);

    if (width == 0 || height == 0 || width % 4 != 0) {
        std::cerr << "Invalid dimensions.\n";
        return 1;
    }

    std::cout << "Sparse matrix size: " << width << " x " << height << " with " << (width * height) / 1024 << " kB\n";

    DATA_T* A = create_array(width * height);
    DATA_T* B = create_array(width * height);
    DATA_T* C = (DATA_T*)malloc(width * height * sizeof(DATA_T));
    DATA_T* vec = create_array(width);
    DATA_T* result = (DATA_T*)malloc(height * sizeof(DATA_T));

    if (A == NULL || B == NULL || C == NULL || vec == NULL || result == NULL) {
        std::cerr << "Couldn't allocate.\n";
        return 2;
    }

    // Time addition
    auto start = std::chrono::high_resolution_clock::now();
    add_sparse_matrices(A, B, C, width, height);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Addition time: " << elapsed.count() << " ms\n";

    // Time multiplication
    start = std::chrono::high_resolution_clock::now();
    multiply_sparse_matrices(A, B, C, width, height);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Multiplication time: " << elapsed.count() << " ms\n";

    // Time vector-matrix multiplication
    start = std::chrono::high_resolution_clock::now();
    vector_matrix_multiplication(A, vec, result, width, height);
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    std::cout << "Vector-Matrix Multiplication time: " << elapsed.count() << " ms\n";

    free(A);
    free(B);
    free(C);
    free(vec);
    free(result);

    return 0;
}
