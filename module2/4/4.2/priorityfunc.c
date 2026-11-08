#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "priority.h"

void initQueue(PriorityQueue *queue) {
    queue->head = NULL;
}

void enqueue(PriorityQueue *queue, int priority, const char *message) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->priority = priority;
    strncpy(newNode->message, message, sizeof(newNode->message) - 1);
    newNode->message[sizeof(newNode->message) - 1] = '\0';
    newNode->next = NULL;

    if (queue->head == NULL || queue->head->priority > priority) {
        newNode->next = queue->head;
        queue->head = newNode;
    } else {
        Node* current = queue->head;
        while (current->next != NULL && current->next->priority <= priority) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

Node* dequeue(PriorityQueue *queue) {
    if (queue->head == NULL) {
        printf("Empty!\n");
        return NULL;
    }
    Node* temp = queue->head;
    queue->head = queue->head->next;
    return temp;
}

Node* dequeueWithPriority(PriorityQueue *queue, int priority) {
    Node* current = queue->head;
    Node* prev = NULL;

    while (current != NULL && current->priority != priority) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Element with priority %d not found!\n", priority);
        return NULL;
    }

    if (prev == NULL) {
        queue->head = current->next;
    } else {
        prev->next = current->next;
    }

    return current;
}

Node* dequeueWithMinPriority(PriorityQueue *queue, int minPriority) {
    Node* current = queue->head;
    Node* prev = NULL;

    while (current != NULL && current->priority > minPriority) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Element with priority no less than %d not found!\n", minPriority);
        return NULL;
    }

    if (prev == NULL) {
        queue->head = current->next;
    } else {
        prev->next = current->next;
    }

    return current;
}

void printQueue(PriorityQueue *queue) {
    printf("Current Queue:\n");
    Node* current = queue->head;
    while (current != NULL) {
        printf("Priority: %d, Message: %s\n", current->priority, current->message);
        current = current->next;
    }
}

void freeNode(Node* node) {
    if (node != NULL) {
        free(node);
    }
}
