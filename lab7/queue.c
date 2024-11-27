#include "queue.h"

Queue* queue_create(uint8_t capacity) {
    Queue* queue = malloc(sizeof(Queue)); 
    if (queue == NULL) 
        return NULL;

    queue->values = malloc(sizeof(uint8_t) * capacity); 
    if (queue->values == NULL) {
        free(queue); 
        return NULL;
    }
    queue->capacity = capacity;
    queue->front = 0;
    //Because rear is unsigned (can't be -1) and push makes rear = rear +1 and first pushed element must be in index 0
    queue->rear = capacity - 1;
    queue->currentSize = 0;

    return queue;
}

bool queue_push(Queue* q, uint8_t element) {
    if (q == NULL || queue_is_full(q))
        return false;
   // printf("Before push size %u\n", q->currentSize);
  //  printf("Element pushed is %u\n", element);
    q->rear = (q->rear + 1) % q->capacity;
    q->values[q->rear] = element;
    q->currentSize++;
   /* for(unsigned i = q->front; i <= q->rear; i++){
        printf("Element %u in the queue is %x\n", i, q->values[i]);
    }*/
    //printf("After push size %u\n", q->currentSize);
    return true;
}

uint8_t queue_pop(Queue* q) {
    if (q == NULL || queue_is_empty(q))
        return 0;

    uint8_t val = queue_front(q);
    q->front = (q->front + 1) % q->capacity;
    q->currentSize--;
 //printf("Element pushed is %u\n", element);
    return val;
}

uint8_t queue_front(const Queue* q) {
    if (q == NULL || queue_is_empty(q))
        return 0;

    return q->values[q->front];
}

bool queue_is_empty(const Queue* q) {
    return (q == NULL || q->currentSize == 0);
}

bool queue_is_full(const Queue* q) {
    return (q != NULL && q->currentSize == q->capacity);
}

void queue_destroy(Queue* q) {
    if (q == NULL)
        return;

    free(q->values); 
    free(q);
}
