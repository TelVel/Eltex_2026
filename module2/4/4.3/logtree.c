#include <stdio.h>
#include "logtree.h"

int main() {
    int cmd = 1;
    int id = 0;
    int Optional;
    int id_cnt = 0;
    Person* root = NULL;
    root = insert(root, 0, "LastName10", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    root = insert(root, 55, "LastName51", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    root = insert(root, 99, "LastName12", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    root = insert(root, 44, "LastName44", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    root = insert(root, 88, "LastName33", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    root = insert(root, 12, "LastName88", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    root = insert(root, 55, "LastName99", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    root = insert(root, 19, "LastName66", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    root = insert(root, 22, "LastName22", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    root = insert(root, 9, "LastName19", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    char FirstName[20];
    char LastName[20];
    char MiddleName[20] = { 0 };
    char Email[40] = { 0 };
    char Workplace[40] = { 0 };
    char Telegram[40] = { 0 };
    char Instagramm[40] = { 0 };
    while (cmd) {
        printf("Menu\n");
        printf("1. Add new contact\n");
        printf("2. Change existing contact\n");
        printf("3. Delete existing contact\n");
        printf("4. View PhoneBook\n");
        printf("5. View Tree\n");
        printf("0. Exit\n");
        printf("Enter Option ");
        scanf("%d", &cmd);
        fseek(stdin, 0, SEEK_END);
        switch (cmd) {
        case 1:
            printf("Enter Last Name: ");
            scanf("%s", LastName);
            printf("Enter First Name: ");
            scanf("%s", FirstName);
            printf("Do you want to write optional fields? [1-yes / 0-no] ");
            scanf("%d", &Optional);
            if (Optional == 1) {
                printf("Enter Middle Name: ");
                scanf("%s", MiddleName);
                printf("Enter Workplace: ");
                scanf("%s", Workplace);
                printf("Enter Email Address: ");
                scanf("%s", Email);
                printf("Enter Telegram: ");
                scanf("%s", Telegram);
                printf("Enter Instagramm: ");
                scanf("%s", Instagramm);
            }
            root = insert(root, id_cnt, LastName, FirstName, MiddleName, Workplace, Email, Telegram, Instagramm);
            id_cnt++;
            break;
        case 2:
            printf("Enter id: ");
            scanf("%d", &id);
            printf("Enter Last Name: ");
            scanf("%s", LastName);
            printf("Enter First Name: ");
            scanf("%s", FirstName);
            printf("Do you want to change optional fields? [1-yes / 0-no] ");
            scanf("%d", &Optional);
            if (Optional == 1) {
                printf("Enter Middle Name: ");
                scanf("%s", MiddleName);
                printf("Enter Workplace: ");
                scanf("%s", Workplace);
                printf("Enter Email Address: ");
                scanf("%s", Email);
                printf("Enter Telegram: ");
                scanf("%s", Telegram);
                printf("Enter Instagramm: \n");
                scanf("%s", Instagramm);
            }
            root = deleteNode(root, id);
            root = insert(root, id, LastName, FirstName, MiddleName, Workplace, Email, Telegram, Instagramm);
            break;
        case 3:
            printf("Enter id: ");
            scanf("%d", &id);
            root = deleteNode(root, id);
            break;
        case 4:
            preOrder(root);
            break;
        case 5:
            printTree(root, 0);
            break;
        case 0:
            break;
        default:
            break;
        }
    }
    return 0;
}
