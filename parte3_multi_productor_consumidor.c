#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 5           
#define NUM_PRODUCERS 3         
#define NUM_CONSUMERS 2         
#define ITEMS_PER_PRODUCER 10   

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
int total_produced = 0;
int total_consumed = 0;
pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;

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
    
    for (int i = 0; i < ITEMS_PER_PRODUCER; i++) {
        int item = producer_id * 1000 + i; // Unique item per producer
        
        printf("Producer %d attempting to produce item %d\n", producer_id, item);
        buffer_insert(item);
        
        pthread_mutex_lock(&stats_mutex);
        total_produced++;
        pthread_mutex_unlock(&stats_mutex);
        
        usleep(50000 + (rand() % 100000)); // Random sleep 50-150ms
    }
    
    printf("=== Producer %d finished. ===\n", producer_id);
    return NULL;
}

void* consumer(void* arg) {
    int consumer_id = *(int*)arg;
    int items_consumed = 0;
    int expected_total = NUM_PRODUCERS * ITEMS_PER_PRODUCER;
    
    while (1) {
        pthread_mutex_lock(&stats_mutex);
        int current_total_produced = total_produced;
        int current_total_consumed = total_consumed;
        pthread_mutex_unlock(&stats_mutex);
        
        // Exit if all items have been produced and consumed
        if (current_total_produced >= expected_total && 
            current_total_consumed >= expected_total) {
            break;
        }
        
        printf("Consumer %d attempting to consume\n", consumer_id);
        int item = buffer_remove();
        
        printf("Consumer %d processed item: %d\n", consumer_id, item);
        items_consumed++;
        
        pthread_mutex_lock(&stats_mutex);
        total_consumed++;
        pthread_mutex_unlock(&stats_mutex);
        
        usleep(75000 + (rand() % 150000)); // Random sleep 75-225ms
    }
    
    printf("=== Consumer %d finished. Items consumed: %d ===\n", consumer_id, items_consumed);
    return NULL;
}

int main() {
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];
    int producer_ids[NUM_PRODUCERS];
    int consumer_ids[NUM_CONSUMERS];
    
    printf("Multi-Producer Multi-Consumer Problem\n");
    printf("Buffer size: %d\n", BUFFER_SIZE);
    printf("Number of producers: %d\n", NUM_PRODUCERS);
    printf("Number of consumers: %d\n", NUM_CONSUMERS);
    printf("Items per producer: %d\n", ITEMS_PER_PRODUCER);
    printf("Total items to produce: %d\n\n", NUM_PRODUCERS * ITEMS_PER_PRODUCER);
    
    buffer_init();
    
    // Create producer threads
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producer_ids[i] = i + 1;
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }
    
    // Create consumer threads
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        consumer_ids[i] = i + 1;
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }
    
    // Wait for producer threads to complete
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }
    
    // Wait for consumer threads to complete
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }
    
    buffer_cleanup();
    
    printf("\n=== FINAL STATISTICS ===\n");
    printf("Total items produced: %d\n", total_produced);
    printf("Total items consumed: %d\n", total_consumed);
    printf("Program completed successfully.\n");
    
    return 0;
}