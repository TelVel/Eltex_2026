#include <stdio.h>
#include <string.h>
#include "mainlog.h"

static int failed;

static void check(int cond, const char *msg) {
    if (!cond) {
        printf("FAIL: %s\n", msg);
        failed++;
    }
}

static Person sample(int id, const char *last, const char *first) {
    Person p;
    memset(&p, 0, sizeof(p));
    addContact(&p, id, (char *)last, (char *)first, "Mid", "Work", "a@b.c", "tg", "ig");
    return p;
}

int main(void) {
    Person book[4];
    Person removed[4];
    Person changed;
    Person blocked;
    memset(&blocked, 0, sizeof(blocked));
    strcpy(blocked.LastName, "keep");

    book[0] = sample(0, "Ivanov", "Ivan");
    book[1] = sample(1, "Petrov", "Petr");
    book[2] = sample(2, "Sidorov", "Sid");

    check(book[0].id == 0 && strcmp(book[0].LastName, "Ivanov") == 0, "add first contact");
    check(strcmp(book[1].FirstName, "Petr") == 0, "add second contact");
    check(strcmp(book[0].Social.EmailAddress, "a@b.c") == 0, "add stores email");
    check(strcmp(book[2].Social.Instagramm, "ig") == 0, "add stores instagram");

    addContact(&blocked, Max_Person, "Nope", "No", "", "", "", "", "");
    check(strcmp(blocked.LastName, "keep") == 0, "add refuses a full book");

    ChangeContact(&changed, 1, "Pavlov", "Pavel", "M", "Lab", "p@p.p", "ptg", "pig");
    check(changed.id == 1 && strcmp(changed.LastName, "Pavlov") == 0, "change last name");
    check(strcmp(changed.Social.Telegram, "ptg") == 0, "change telegram");

    DeleteContact(removed, book, 1, 3);
    check(removed[0].id == 0 && strcmp(removed[0].LastName, "Ivanov") == 0, "delete keeps the head");
    check(removed[1].id == 1 && strcmp(removed[1].LastName, "Sidorov") == 0, "delete shifts the tail");

    ViewContact(book, 3);

    if (failed) {
        printf("%d test(s) failed\n", failed);
        return 1;
    }
    printf("all tests passed\n");
    return 0;
}
