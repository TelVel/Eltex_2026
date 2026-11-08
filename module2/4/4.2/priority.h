#ifndef PRIORITY_H
#define PRIORITY_H

#define MAX_PRIORITY 255

typedef struct Node {
    int priority;
    char message[256];
    struct Node* next;
} Node;

typedef struct {
    Node* head;
} PriorityQueue;

void initQueue(PriorityQueue *queue);
void enqueue(PriorityQueue *queue, int priority, const char *message);
Node* dequeue(PriorityQueue *queue);
Node* dequeueWithPriority(PriorityQueue *queue, int priority);
Node* dequeueWithMinPriority(PriorityQueue *queue, int minPriority);
void printQueue(PriorityQueue *queue);
void freeNode(Node* node);

#endif
