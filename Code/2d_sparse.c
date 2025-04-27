// This is a replacement main for your array-2d.c...

// Get modern behaviour out of time.h, per https://stackoverflow.com/a/40515669
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

/*
#define DATA_T int64_t
#define DATA_PRINTF "%ld"
#define RAND_EXPR (rand() % 256 - 127)
*/
#define DATA_T double
#define DATA_PRINTF "%g"
#define RAND_EXPR ((double)rand() / RAND_MAX - 0.5)


DATA_T* create_sparse_array(uint64_t width, uint64_t height, uint64_t num_elements) {
    // Initialize random number generator
    srand(time(NULL));

    // Calculate total size
    uint64_t size = width * height;

    // Allocate memory for the array
    DATA_T* array = (DATA_T*)calloc(size, sizeof(DATA_T));
    if (array == NULL) {
        return NULL;
    }

    // Randomly assign non-zero values
    for (uint64_t i = 0; i < num_elements; i++) {
        uint64_t pos;
        do {
            pos = rand() % size;  // Get a random position in the array
        } while (array[pos] != 0);  // Ensure we don't overwrite an existing non-zero element

        array[pos] = RAND_EXPR;  // Assign a random value
    }

    return array;
}

// Function to add two sparse matrices
DATA_T* add_sparse_matrices(DATA_T* matrix1, DATA_T* matrix2, uint64_t size) {
    DATA_T* result = (DATA_T*)malloc(size * sizeof(DATA_T));
    if (result == NULL) {
        return NULL;
    }
    for (uint64_t i = 0; i < size; i++) {
        result[i] = matrix1[i] + matrix2[i];
    }
    return result;
}

// Function to multiply two sparse matrices
DATA_T* multiply_sparse_matrices(DATA_T* matrix1, DATA_T* matrix2, uint64_t width, uint64_t height) {
    DATA_T* result = (DATA_T*)calloc(width * height, sizeof(DATA_T));
    if (result == NULL) {
        return NULL;
    }
    for (uint64_t i = 0; i < height; i++) {
        for (uint64_t j = 0; j < width; j++) {
            for (uint64_t k = 0; k < width; k++) {
                result[i * width + j] += matrix1[i * width + k] * matrix2[k * width + j];
            }
        }
    }
    return result;
}

// Function to multiply a sparse matrix by a vector
DATA_T* multiply_matrix_vector(DATA_T* matrix, DATA_T* vector, uint64_t width, uint64_t height) {
    DATA_T* result = (DATA_T*)calloc(height, sizeof(DATA_T));
    if (result == NULL) {
        return NULL;
    }
    for (uint64_t i = 0; i < height; i++) {
        for (uint64_t j = 0; j < width; j++) {
            result[i] += matrix[i * width + j] * vector[j];
        }
    }
    return result;
}

typedef struct {
    DATA_T result;
    double elapsed_ms;
} time_result;

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


// Main function
int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Must give array size, height, and number of non-zero elements on command line.\n");
        return 1;
    }
    uint64_t n = atol(argv[1]);
    uint64_t h = atol(argv[2]);
    uint64_t num_elements = atol(argv[3]);
    printf("Array size: %lu kB\n", n * sizeof(DATA_T) / 1024);

    if ( n % h != 0 ) {
        printf("n not divisible by h\n");
        return 1;
    }

    DATA_T* array1 = create_sparse_array(n, num_elements);
    if (array1 == NULL) {
        printf("Couldn't allocate array1.\n");
        return 2;
    }

    DATA_T* array2 = create_sparse_array(n, num_elements);
    if (array2 == NULL) {
        printf("Couldn't allocate array2.\n");
        free(array1);
        return 2;
    }

    time_result* res;

    uint64_t w = n / h;
    // treat array like it's h*w...
    res = time_it(sum_array_col, array1, w, h);
    printf("Calculated " DATA_PRINTF " in %8.2f ms on %lu*%lu array1.\n", res->result, res->elapsed_ms, w, h);
    free(res);

    res = time_it(sum_array_col, array2, w, h);
    printf("Calculated " DATA_PRINTF " in %8.2f ms on %lu*%lu array2.\n", res->result, res->elapsed_ms, w, h);
    free(res);

    DATA_T* sum = add_sparse_matrices(array1, array2, n);
    DATA_T* product = multiply_sparse_matrices(array1, array2, w, h);
    DATA_T* vector = create_array(w);
    DATA_T* matrix_vector_product = multiply_matrix_vector(array1, vector, w, h);

    // Remember to free the allocated memory
    free(array1);
    free(array2);
    free(sum);
    free(product);
    free(vector);
    free(matrix_vector_product);

    return 0;
}
