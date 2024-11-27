#ifndef QUEUE_H
#define QUEUE_H

#include <lcom/lcf.h>


typedef struct Queue {
    uint8_t* values;
    uint8_t front;
    uint8_t rear;
    uint8_t capacity;
    uint8_t currentSize;
} Queue;

/**
 * @brief Creates a new queue with the specified capacity.
 * 
 * @param size The capacity of the queue.
 * @return A pointer to the newly created queue, or NULL if creation fails.
 */
Queue* queue_create(uint8_t capacity);

/**
 * @brief Enqueues an element into the queue.
 * 
 * @param q Pointer to the queue.
 * @param element The element to be pushed.
 * @return true if the element was successfully pushed, false otherwise.
 */
bool queue_push(Queue* q, uint8_t element);

/**
 * @brief Dequeues an element from the queue.
 * 
 * @param q Pointer to the queue.
 * @return The popped element, or 0 if the queue is empty.
 */
uint8_t queue_pop(Queue* q);

/**
 * @brief Retrieves the front element of the queue.
 * 
 * @param q Pointer to the queue.
 * @return The front element of the queue, or 0 if the queue is empty.
 */
uint8_t queue_front(const Queue* q);

/**
 * @brief Checks if the queue is empty.
 * 
 * @param q Pointer to the queue.
 * @return true if the queue is empty, false otherwise.
 */
bool queue_is_empty(const Queue* q);

/**
 * @brief Checks if the queue is full.
 * 
 * @param q Pointer to the queue.
 * @return true if the queue is full, false otherwise.
 */
bool queue_is_full(const Queue* q);

/**
 * @brief Destroys the queue and frees memory.
 * 
 * @param q Pointer to the queue to be destroyed.
 */
void queue_destroy(Queue* q);

#endif /* QUEUE_H */
