#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 1
#define NUM_ITEMS 20

typedef struct {
    int buffer[BUFFER_SIZE];
    int front;
    int rear;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} CircularBuffer;

CircularBuffer cb;

void buffer_init() {
    cb.front = 0;
    cb.rear = 0;
    cb.count = 0;
    pthread_mutex_init(&cb.mutex, NULL);
    pthread_cond_init(&cb.not_full, NULL);
    pthread_cond_init(&cb.not_empty, NULL);
}

void buffer_cleanup() {
    pthread_mutex_destroy(&cb.mutex);
    pthread_cond_destroy(&cb.not_full);
    pthread_cond_destroy(&cb.not_empty);
}

void buffer_insert(int item) {
    pthread_mutex_lock(&cb.mutex);
    
    // Wait while buffer is full
    while (cb.count == BUFFER_SIZE) {
        printf("Buffer full. Producer waiting...\n");
        pthread_cond_wait(&cb.not_full, &cb.mutex);
    }
    
    // Insert item
    cb.buffer[cb.rear] = item;
    cb.rear = (cb.rear + 1) % BUFFER_SIZE;
    cb.count++;
    
    printf("Produced: %d (Buffer count: %d)\n", item, cb.count);
    
    // Signal that buffer is not empty
    pthread_cond_signal(&cb.not_empty);
    pthread_mutex_unlock(&cb.mutex);
}

int buffer_remove() {
    pthread_mutex_lock(&cb.mutex);
    
    // Wait while buffer is empty
    while (cb.count == 0) {
        printf("Buffer empty. Consumer waiting...\n");
        pthread_cond_wait(&cb.not_empty, &cb.mutex);
    }
    
    // Remove item
    int item = cb.buffer[cb.front];
    cb.front = (cb.front + 1) % BUFFER_SIZE;
    cb.count--;
    
    printf("Consumed: %d (Buffer count: %d)\n", item, cb.count);
    
    // Signal that buffer is not full
    pthread_cond_signal(&cb.not_full);
    pthread_mutex_unlock(&cb.mutex);
    
    return item;
}

void* producer(void* arg) {
    int producer_id = *(int*)arg;
    
    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = producer_id * 1000 + i; // Unique item per producer
        buffer_insert(item);
        usleep(100000); // Sleep 100ms to simulate work
    }
    
    printf("Producer %d finished.\n", producer_id);
    return NULL;
}

void* consumer(void* arg) {
    int consumer_id = *(int*)arg;
    
    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = buffer_remove();
        printf("Consumer %d processed item: %d\n", consumer_id, item);
        usleep(150000); // Sleep 150ms to simulate processing
    }
    
    printf("Consumer %d finished.\n", consumer_id);
    return NULL;
}

int main() {
    pthread_t producer_thread, consumer_thread;
    int producer_id = 1, consumer_id = 1;
    
    printf("Producer-Consumer Problem (Single Producer, Single Consumer)\n");
    printf("Buffer size: %d\n", BUFFER_SIZE);
    printf("Items to produce/consume: %d\n\n", NUM_ITEMS);
    
    buffer_init();
    
    // Create producer and consumer threads
    pthread_create(&producer_thread, NULL, producer, &producer_id);
    pthread_create(&consumer_thread, NULL, consumer, &consumer_id);
    
    // Wait for threads to complete
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
    
    buffer_cleanup();
    
    printf("\nProgram completed successfully.\n");
    return 0;
}