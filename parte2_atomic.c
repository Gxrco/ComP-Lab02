#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define ARRAY_SIZE 1000000

int main() {
    int *array = malloc(ARRAY_SIZE * sizeof(int));
    long long sum = 0;
    double start_time, end_time;
    int num_threads = 4;
    
    // Initialize array with random values
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = rand() % 100 + 1; // Values between 1 and 100
    }
    
    printf("Array Sum using ATOMIC\n");
    printf("Array size: %d elements\n", ARRAY_SIZE);
    printf("Number of threads: %d\n\n", num_threads);
    
    omp_set_num_threads(num_threads);
    
    start_time = omp_get_wtime();
    
    // Parallel sum using atomic operations
    #pragma omp parallel for
    for (int i = 0; i < ARRAY_SIZE; i++) {
        #pragma omp atomic
        sum += array[i];
    }
    
    end_time = omp_get_wtime();
    
    printf("Sum result: %lld\n", sum);
    printf("Execution time with atomic: %f seconds\n", end_time - start_time);
    
    // Verify with sequential calculation
    long long sequential_sum = 0;
    start_time = omp_get_wtime();
    for (int i = 0; i < ARRAY_SIZE; i++) {
        sequential_sum += array[i];
    }
    end_time = omp_get_wtime();
    
    printf("Sequential sum: %lld\n", sequential_sum);
    printf("Sequential time: %f seconds\n", end_time - start_time);
    printf("Results match: %s\n", (sum == sequential_sum) ? "YES" : "NO");
    
    free(array);
    return 0;
}