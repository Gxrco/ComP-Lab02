#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define ARRAY_SIZE 100000

int main() {
    double *array = malloc(ARRAY_SIZE * sizeof(double));
    double start_time, end_time;
    
    // Parameters to modify for testing
    int num_threads = 8;
    int chunk_size = 1000;
    char scheduler[] = "guided";
    
    // Initialize array with random values
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = (double)(rand() % 1000) + 1.0;
    }
    
    printf("Parallel Square Root Calculation\n");
    printf("Array size: %d elements\n", ARRAY_SIZE);
    printf("Number of threads: %d\n", num_threads);
    printf("Scheduler: %s\n", scheduler);
    printf("Chunk size: %d\n\n", chunk_size);
    
    // Set number of threads
    omp_set_num_threads(num_threads);
    
    start_time = omp_get_wtime();
    
    // Parallel execution with different schedulers
    // Uncomment the appropriate scheduler:
    
    // GUIDED scheduler
    #pragma omp parallel for schedule(guided, chunk_size)
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = sqrt(array[i]);
    }
    
    /* DYNAMIC scheduler - uncomment to test
    #pragma omp parallel for schedule(dynamic, chunk_size)
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = sqrt(array[i]);
    }
    */
    
    /* GUIDED scheduler - uncomment to test
    #pragma omp parallel for schedule(guided, chunk_size)
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = sqrt(array[i]);
    }
    */
    
    end_time = omp_get_wtime();
    
    printf("Parallel execution time: %f seconds\n", end_time - start_time);
    
    // Optional: Print first 10 results for verification
    printf("\nFirst 10 results:\n");
    for (int i = 0; i < 10; i++) {
        printf("array[%d] = %.4f\n", i, array[i]);
    }
    
    free(array);
    return 0;
}