#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define DATA_T double
#define DATA_PRINTF "%g"
#define RAND_EXPR ((double)rand() / RAND_MAX - 0.5)

DATA_T* create_sparse_matrix(uint64_t width, uint64_t height, uint64_t num_elements) {
    DATA_T* matrix = (DATA_T*)calloc(width * height, sizeof(DATA_T));
    if (matrix == NULL) {
        return NULL;
    }
    for (uint64_t i = 0; i < num_elements; i++) {
        uint64_t row = rand() % height;
        uint64_t col = rand() % width;
        matrix[row * width + col] = RAND_EXPR;
    }
    return matrix;
}

DATA_T* add_sparse_matrices(DATA_T* A, DATA_T* B, uint64_t width, uint64_t height) {
    DATA_T* C = (DATA_T*)malloc(width * height * sizeof(DATA_T));
    if (C == NULL) {
        return NULL;
    }
    for (uint64_t i = 0; i < width * height; i++) {
        C[i] = A[i] + B[i];
    }
    return C;
}
/*
DATA_T* multiply_sparse_matrices(DATA_T* A, DATA_T* B, uint64_t width, uint64_t height) {
    DATA_T* C = (DATA_T*)calloc(width * height, sizeof(DATA_T));
    if (C == NULL) {
        return NULL;
    }
    for (uint64_t i = 0; i < height; i++) {
        for (uint64_t j = 0; j < width; j++) {
            for (uint64_t k = 0; k < width; k++) {
                C[i * width + j] += A[i * width + k] * B[k * width + j];
            }
        }
    }
    return C;
} */

DATA_T* multiply_sparse_matrices(DATA_T* A, DATA_T* B, uint64_t width, uint64_t height) {
    DATA_T* C = (DATA_T*)calloc(width * height, sizeof(DATA_T));
    if (C == NULL) {
        return NULL;
    }
    for (uint64_t i = 0; i < height; i++) {
        for (uint64_t j = 0; j < width; j++) {
            int random_branch = rand() % 2; // Random branch
            if (random_branch) {
                for (uint64_t k = 0; k < width; k++) {
                    C[i * width + j] += A[i * width + k] * B[k * width + j];
                }
            } else {
                // Adding another unpredictable path
                for (uint64_t k = 0; k < width; k++) {
                    C[i * width + j] += A[i * width + k] * B[k * width + j];
                }
            }
        }
    }
    return C;
}


DATA_T* multiply_vector_by_matrix(DATA_T* vector, DATA_T* matrix, uint64_t width, uint64_t height) {
    DATA_T* result = (DATA_T*)malloc(height * sizeof(DATA_T));
    if (result == NULL) {
        return NULL;
    }
    for (uint64_t i = 0; i < height; i++) {
        result[i] = 0;
        for (uint64_t j = 0; j < width; j++) {
            result[i] += vector[j] * matrix[i * width + j];
        }
    }
    return result;
}

typedef struct {
    DATA_T result;
    double elapsed_ms;
} time_result;

typedef struct {
    DATA_T* result;
    double elapsed_ms;
} time_result_matrix;

time_result* time_it(DATA_T(*sum_array)(DATA_T*, uint64_t, uint64_t), DATA_T* array, uint64_t width, uint64_t height) {
    struct timespec start, end;
    time_result* res = (time_result*)malloc(sizeof(time_result));

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    DATA_T total = sum_array(array, width, height);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    res->result = total;
    res->elapsed_ms = elapsed * 1000;
    return res;
}

time_result_matrix* time_it_matrix(DATA_T* (*matrix_op)(DATA_T*, DATA_T*, uint64_t, uint64_t), DATA_T* A, DATA_T* B, uint64_t width, uint64_t height) {
    struct timespec start, end;
    time_result_matrix* res = (time_result_matrix*)malloc(sizeof(time_result_matrix));

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    DATA_T* result = matrix_op(A, B, width, height);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    res->result = result;
    res->elapsed_ms = elapsed * 1000;
    return res;
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        printf("Usage: %s width height num_elements vector_size\n", argv[0]);
        return 1;
    }
    uint64_t width = atol(argv[1]);
    uint64_t height = atol(argv[2]);
    uint64_t num_elements = atol(argv[3]);
    uint64_t vector_size = atol(argv[4]);

    printf("Sparse matrix size: %lu x %lu with %lu non-zero elements\n", width, height, num_elements);

    DATA_T* A = create_sparse_matrix(width, height, num_elements);
    DATA_T* B = create_sparse_matrix(width, height, num_elements);
    DATA_T* vector = create_sparse_matrix(vector_size, 1, vector_size); // Creating a sparse vector

    if (A == NULL || B == NULL || vector == NULL) {
        printf("Couldn't allocate memory.\n");
        return 2;
    }

    time_result_matrix* res_add = time_it_matrix(add_sparse_matrices, A, B, width, height);
    time_result_matrix* res_mult = time_it_matrix(multiply_sparse_matrices, A, B, width, height);
    time_result_matrix* res_vec_mult = time_it_matrix((DATA_T* (*)(DATA_T*, DATA_T*, uint64_t, uint64_t))multiply_vector_by_matrix, vector, A, width, height);

    printf("Addition time: %8.2f ms\n", res_add->elapsed_ms);
    printf("Multiplication time: %8.2f ms\n", res_mult->elapsed_ms);
    printf("Vector-Matrix Multiplication time: %8.2f ms\n", res_vec_mult->elapsed_ms);

    free(res_add->result);
    free(res_mult->result);
    free(res_vec_mult->result);
    free(res_add);
    free(res_mult);
    free(res_vec_mult);
    free(A);
    free(B);
    free(vector);
    return 0;
}
