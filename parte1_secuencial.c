#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define ARRAY_SIZE 100000

int main() {
    double *array = malloc(ARRAY_SIZE * sizeof(double));
    clock_t start, end;
    double cpu_time_used;
    
    // Initialize array with random values
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = (double)(rand() % 1000) + 1.0; // Random values between 1 and 1000
    }
    
    printf("Sequential Square Root Calculation\n");
    printf("Array size: %d elements\n\n", ARRAY_SIZE);
    
    // Measure execution time
    start = clock();
    
    // Apply square root to each element
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = sqrt(array[i]);
    }
    
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Sequential execution time: %f seconds\n", cpu_time_used);
    
    // Optional: Print first 10 results for verification
    printf("\nFirst 10 results:\n");
    for (int i = 0; i < 10; i++) {
        printf("array[%d] = %.4f\n", i, array[i]);
    }
    
    free(array);
    return 0;
}