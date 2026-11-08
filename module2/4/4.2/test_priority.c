#include <stdio.h>
#include <string.h>
#include "priority.h"

static int failed;

static void check(int cond, const char *msg) {
    if (!cond) {
        printf("FAIL: %s\n", msg);
        failed++;
    }
}

static void fill(PriorityQueue *queue) {
    initQueue(queue);
    enqueue(queue, 10, "Message 1");
    enqueue(queue, 200, "Message 2");
    enqueue(queue, 50, "Message 3");
    enqueue(queue, 150, "Message 4");
    enqueue(queue, 255, "Message 5");
}

static void drain(PriorityQueue *queue) {
    Node *node;
    while ((node = dequeue(queue)) != NULL) {
        freeNode(node);
    }
}

int main(void) {
    PriorityQueue queue;
    Node *node;
    const int order[] = {10, 50, 150, 200, 255};
    int i = 0;

    fill(&queue);
    for (Node *cur = queue.head; cur != NULL; cur = cur->next, i++) {
        check(cur->priority == order[i], "queue stays ordered by priority");
    }
    check(i == 5, "five seeded messages");
    check(strcmp(queue.head->message, "Message 1") == 0, "smallest priority is first");

    node = dequeue(&queue);
    check(node != NULL && node->priority == 10, "dequeue takes the head");
    freeNode(node);
    check(queue.head->priority == 50, "next head is 50");

    node = dequeueWithPriority(&queue, 150);
    check(node != NULL && strcmp(node->message, "Message 4") == 0, "extract priority 150");
    freeNode(node);
    check(dequeueWithPriority(&queue, 150) == NULL, "missing priority is not found");

    fill(&queue);
    node = dequeueWithMinPriority(&queue, 100);
    check(node != NULL && node->priority == 10, "minimum bound returns the first match");
    freeNode(node);
    node = dequeueWithMinPriority(&queue, 9);
    check(node == NULL, "bound below every priority finds nothing");

    enqueue(&queue, 40, "Inserted");
    check(queue.head->priority == 40, "insert before a larger head");
    check(strcmp(queue.head->message, "Inserted") == 0, "inserted message is kept");

    printQueue(&queue);
    drain(&queue);
    check(dequeue(&queue) == NULL, "empty dequeue");

    if (failed) {
        printf("%d test(s) failed\n", failed);
        return 1;
    }
    printf("all tests passed\n");
    return 0;
}
