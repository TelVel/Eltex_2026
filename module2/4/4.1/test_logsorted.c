#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logsorted.h"

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
    char last[20] = "Aaa";
    char first[20] = "New";
    char middle[20] = "M";
    char work[40] = "NewCo";
    char email[40] = "new@mail";
    char tg[40] = "newtg";
    char ig[40] = "newig";
    Node *found;

    initList(&list);
    check(same_ids(&list, initial, 10), "initial list is sorted by last name");
    check(strcmp(list.head->contact.LastName, "LastName10") == 0, "first last name");
    check(strcmp(list.tail->contact.LastName, "LastName99") == 0, "last last name");

    addContact(&list, 100, last, first, middle, work, email, tg, ig);
    check(list.head->contact.id == 100, "new name is inserted at the head");
    check(list.head->next->prev == list.head, "head link stays doubly linked");

    deleteContact(&list, 100);
    check(getContactById(&list, 100) == NULL, "deleted contact is gone");
    check(same_ids(&list, initial, 10), "delete restores the initial order");

    deleteContact(&list, 404);
    check(same_ids(&list, initial, 10), "missing delete does nothing");

    strcpy(last, "Zzz");
    strcpy(first, "Changed");
    changeContact(&list, 0, last, first, middle, work, email, tg, ig, 0);
    found = getContactById(&list, 0);
    check(found != NULL && strcmp(found->contact.LastName, "Zzz") == 0, "change moves the last name");
    check(found != NULL && strcmp(found->contact.FirstName, "Changed") == 0, "change updates the first name");
    check(found != NULL && strcmp(found->contact.Social.EmailAddress, "0@gmail.com") == 0, "optional fields stay without the flag");
    check(list.tail->contact.id == 0, "changed name becomes the tail");

    strcpy(email, "changed@mail");
    changeContact(&list, 0, last, first, middle, work, email, tg, ig, 1);
    found = getContactById(&list, 0);
    check(found != NULL && strcmp(found->contact.Social.EmailAddress, "changed@mail") == 0, "optional fields update with the flag");

    changeContact(&list, 404, last, first, middle, work, email, tg, ig, 1);
    check(getContactById(&list, 404) == NULL, "change of a missing id does nothing");

    viewContacts(&list);
    free_list(&list);

    if (failed) {
        printf("%d test(s) failed\n", failed);
        return 1;
    }
    printf("all tests passed\n");
    return 0;
}
