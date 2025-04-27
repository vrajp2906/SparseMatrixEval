#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#define DATA_T uint64_t
#define DATA_PRINTF "%lu"
#define RAND_EXPR (rand() % 100 + 1)  // Generate positive whole numbers from 1 to 100

typedef struct {
    uint64_t row;
    uint64_t col;
    DATA_T value;
} Element;

Element* create_sparse_array(uint64_t length, uint64_t matrix_size) {
    Element* array = (Element*)malloc(length * sizeof(Element));
    if (array == NULL) {
        return NULL;
    }

    // Generate a set of unique positions
    uint64_t* positions = (uint64_t*)malloc(length * sizeof(uint64_t));
    if (positions == NULL) {
        free(array);
        return NULL;
    }
    for (uint64_t i = 0; i < length; i++) {
        uint64_t pos;
        do {
            pos = rand() % (matrix_size * matrix_size);
            uint64_t j;
            for (j = 0; j < i; j++) {
                if (positions[j] == pos) {
                    break;
                }
            }
            if (j == i) {
                positions[i] = pos;
                break;
            }
        } while (1);
    }

    // Assign values to the unique positions
    for (uint64_t i = 0; i < length; i++) {
        array[i].row = positions[i] / matrix_size;
        array[i].col = positions[i] % matrix_size;
        array[i].value = RAND_EXPR;
    }

    free(positions);
    return array;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <number of elements> <matrix size>\n", argv[0]);
        return 1;
    }

    srand(time(NULL));

    uint64_t length = atoll(argv[1]);
    uint64_t matrix_size = atoll(argv[2]);
    Element* sparse_array = create_sparse_array(length, matrix_size);

    if (sparse_array == NULL) {
        printf("Failed to allocate memory for sparse array\n");
        return 1;
    }

    for (uint64_t i = 0; i < length; i++) {
        printf("Element at (%lu, %lu) = " DATA_PRINTF "\n", sparse_array[i].row, sparse_array[i].col, sparse_array[i].value);
    }

    free(sparse_array);

    return 0;
}
