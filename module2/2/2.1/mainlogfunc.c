#include <stdio.h>
#include <string.h>
#include "mainlog.h"

void addContact(Person* contactlist, int cnt, char* LastName, char* FirstName, char* MiddleName, char* Workplace, char* Email, char* Telegram, char* Instagramm) {
    if (cnt >= Max_Person)
    {
        return;
    }
    Person NewCont;
    NewCont.id = cnt;
    strncpy(NewCont.LastName, LastName, 20);
    strncpy(NewCont.FirstName, FirstName, 20);
    strncpy(NewCont.MiddleName, MiddleName, 20);
    strncpy(NewCont.Workplace, Workplace, 40);
    strncpy(NewCont.Social.EmailAddress, Email, 40);
    strncpy(NewCont.Social.Telegram, Telegram, 40);
    strncpy(NewCont.Social.Instagramm, Instagramm, 40);
    *contactlist = NewCont;
}

void DeleteContact(Person* Chdel, Person contactlist[], int id, int cnt) {
    Person Del[Max_Person];
    int i = 0;
    for (i; i < id; i++) { Del[i] = contactlist[i]; }
    for (i = id; i < cnt - 1; i++) { Del[i] = contactlist[i + 1]; Del[i].id = i;}
    for (i = 0; i < cnt - 1; i++) { Chdel[i] = Del[i]; }
}

void ChangeContact(Person* contactlist, int id, char* LastName, char* FirstName, char* MiddleName, char* Workplace, char* Email, char* Telegram, char* Instagramm)
{
    Person ChCont;
    ChCont.id = id;
    strncpy(ChCont.LastName, LastName, 20);
    strncpy(ChCont.FirstName, FirstName, 20);
    strncpy(ChCont.MiddleName, MiddleName, 20);
    strncpy(ChCont.Workplace, Workplace, 40);
    strncpy(ChCont.Social.EmailAddress, Email, 40);
    strncpy(ChCont.Social.Telegram, Telegram, 40);
    strncpy(ChCont.Social.Instagramm, Instagramm, 40);
    *contactlist = ChCont;
}

void ViewContact(Person contactlist[], int cnt) {
    for (int i = 0; i < cnt; i++)
    {
        printf("id: %d ", contactlist[i].id);
        printf("Last Name: %s ", contactlist[i].LastName);
        printf("First Name: %s ", contactlist[i].FirstName);
        printf("Middle Name: %s ", contactlist[i].MiddleName);
        printf("Workplace: %s ", contactlist[i].Workplace);
        printf("Email Address: %s ", contactlist[i].Social.EmailAddress);
        printf("Telegram: %s ", contactlist[i].Social.Telegram);
        printf("Instagramm: %s \n", contactlist[i].Social.Instagramm);
    }
}
