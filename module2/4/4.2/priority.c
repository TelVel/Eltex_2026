#include <stdio.h>
#include <string.h>
#include "priority.h"

void displayMenu() {
    printf("\nPriority Queue Menu\n");
    printf("1. Add new message\n");
    printf("2. Extract highest priority message\n");
    printf("3. Extract message with specific priority\n");
    printf("4. Extract message with minimum priority\n");
    printf("5. View all messages\n");
    printf("0. Exit\n");
    printf("Enter your choice: ");
}

int main() {
    PriorityQueue queue;
    initQueue(&queue);
    enqueue(&queue, 10, "Message 1");
    enqueue(&queue, 200, "Message 2");
    enqueue(&queue, 50, "Message 3");
    enqueue(&queue, 150, "Message 4");
    enqueue(&queue, 255, "Message 5");
    int choice;
    int priority;
    char message[256];
    Node* element;

    do {
        displayMenu();
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                printf("Enter priority (0-255): ");
                scanf("%d", &priority);
                getchar();
                printf("Enter message: ");
                fgets(message, sizeof(message), stdin);
                message[strcspn(message, "\n")] = '\0';
                enqueue(&queue, priority, message);
                printf("Message added successfully!\n");
                break;

            case 2:
                element = dequeue(&queue);
                if (element != NULL) {
                    printf("Extracted message: Priority %d - '%s'\n",
                           element->priority, element->message);
                    freeNode(element);
                }
                break;

            case 3:
                printf("Enter priority to extract: ");
                scanf("%d", &priority);
                element = dequeueWithPriority(&queue, priority);
                if (element != NULL) {
                    printf("Extracted message: Priority %d - '%s'\n",
                           element->priority, element->message);
                    freeNode(element);
                }
                break;

            case 4:
                printf("Enter minimum priority: ");
                scanf("%d", &priority);
                element = dequeueWithMinPriority(&queue, priority);
                if (element != NULL) {
                    printf("Extracted message: Priority %d - '%s'\n",
                           element->priority, element->message);
                    freeNode(element);
                }
                break;

            case 5:
                printQueue(&queue);
                break;

            case 0:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 0);

    while (element = dequeue(&queue)) {
        freeNode(element);
    }
}
