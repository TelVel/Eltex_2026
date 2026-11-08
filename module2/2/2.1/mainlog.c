#include <stdio.h>
#include <string.h>
#include "mainlog.h"

int main() {
    int cmd = 1;
    int id = 0;
    int Optional;
    int id_cnt = 0;
    Person clist[Max_Person];
    int cnt = 0;
    char FirstName[20];
    char LastName[20];
    char MiddleName[20] = { 0 };
    char Email[40] = { 0 };
    char Workplace[40] = { 0 };
    char Telegram[40] = { 0 };
    char Instagramm[40] = {0};
    while (cmd)
    {
        printf("Menu\n");
        printf("1. Add new contact\n");
        printf("2. Change existing contact\n");
        printf("3. Delete existing contact\n");
        printf("4. View PhoneBook\n");
        printf("0. Exit\n");
        printf("Enter Option ");
        scanf("%d", &cmd);
        fseek(stdin, 0, SEEK_END);
        switch (cmd) {
        case 1:
            Person result;
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
            addContact(&result, id_cnt, LastName, FirstName, MiddleName, Workplace, Email, Telegram, Instagramm);
            if (Optional == 1)
            {
                strncpy(MiddleName, "", 20);
                strncpy(Workplace, "", 40);
                strncpy(Email, "", 40);
                strncpy(Telegram, "", 40);
                strncpy(Instagramm, "", 40);
            }
            clist[cnt] = result;
            cnt++;
            id_cnt++;
            break;
        case 2:
            Person chRes;
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
            ChangeContact(&chRes, id, LastName, FirstName, MiddleName, Workplace, Email, Telegram, Instagramm);
            strncpy(clist[id].LastName, chRes.LastName, 20);
            strncpy(clist[id].FirstName, chRes.FirstName, 20);
            if (Optional == 1)
            {
                strncpy(clist[id].MiddleName, chRes.MiddleName, 20);
                strncpy(clist[id].Workplace, chRes.Workplace, 40);
                strncpy(clist[id].Social.EmailAddress, chRes.Social.EmailAddress, 40);
                strncpy(clist[id].Social.Telegram, chRes.Social.Telegram, 40);
                strncpy(clist[id].Social.Instagramm, chRes.Social.Instagramm, 40);
            }
            break;
        case 3:
            Person chDel[Max_Person];
            printf("Enter id: ");
            scanf("%d", &id);
            DeleteContact(chDel, clist, id, cnt);
            for (int i = 0; i < cnt - 1; i++) {
                clist[i] = chDel[i];
            }
            cnt--;
            break;
        case 4:
            ViewContact(clist, cnt);
            break;
        case 0:
            break;
        default:
            break;
        }
    }
}
