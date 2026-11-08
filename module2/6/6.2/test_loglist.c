#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "loglist.h"

static int failed;

static void check(int cond, const char *msg) {
    if (!cond) {
        printf("FAIL: %s\n", msg);
        failed++;
    }
}

static void free_list(DoublyLinkedList *list) {
    Node *cur = list->head;
    while (cur) {
        Node *next = cur->next;
        free(cur);
        cur = next;
    }
    list->head = list->tail = NULL;
}

static Node *find_id(DoublyLinkedList *list, int id) {
    Node *cur = list->head;
    while (cur != NULL) {
        if (cur->contact.id == id) return cur;
        cur = cur->next;
    }
    return NULL;
}

static int same_ids(DoublyLinkedList *list, const int *expect, int n) {
    Node *cur = list->head;
    for (int i = 0; i < n; i++) {
        if (cur == NULL || cur->contact.id != expect[i]) return 0;
        if (cur->next != NULL && cur->next->prev != cur) return 0;
        cur = cur->next;
    }
    return cur == NULL && list->tail != NULL && list->tail->contact.id == expect[n - 1];
}

int main(void) {
    DoublyLinkedList list;
    const int initial[] = {0, 8, 7, 2, 5, 3, 9, 4, 1, 6};
    Node *found;

    initList(&list);
    check(same_ids(&list, initial, 10), "initial list is sorted by last name");

    addContact(&list, 100, "Aaa", "New", "M", "NewCo", "new@mail", "newtg", "newig");
    check(list.head->contact.id == 100, "new name is inserted at the head");
    check(strcmp(list.head->contact.Social.Telegram, "newtg") == 0, "add stores telegram");

    deleteContact(&list, 100);
    check(find_id(&list, 100) == NULL, "deleted contact is gone");
    check(same_ids(&list, initial, 10), "delete restores the initial order");
    deleteContact(&list, 404);
    check(same_ids(&list, initial, 10), "missing delete does nothing");

    changeContact(&list, 0, "Zzz", "Changed", "M", "NewCo", "changed@mail", "tg", "ig");
    found = find_id(&list, 0);
    check(found != NULL && strcmp(found->contact.LastName, "Zzz") == 0, "change updates the last name");
    check(found != NULL && strcmp(found->contact.Social.EmailAddress, "changed@mail") == 0, "change replaces the email");
    check(list.tail->contact.id == 0, "changed name becomes the tail");

    viewContacts(&list);
    free_list(&list);

    if (failed) {
        printf("%d test(s) failed\n", failed);
        return 1;
    }
    printf("all tests passed\n");
    return 0;
}
