#ifndef MAINLOG_H
#define MAINLOG_H

#define Max_Person 50

typedef struct Socials {
    char EmailAddress[40];
    char Telegram[40];
    char Instagramm[40];
} Socials;

typedef struct Person {
    int id;
    char LastName[20];
    char FirstName[20];
    char MiddleName[20];
    char Workplace[40];
    struct Socials Social;
} Person;

void addContact(Person* contactlist, int cnt, char* LastName, char* FirstName, char* MiddleName, char* Workplace, char* Email, char* Telegram, char* Instagramm);
void DeleteContact(Person* Chdel, Person contactlist[], int id, int cnt);
void ChangeContact(Person* contactlist, int id, char* LastName, char* FirstName, char* MiddleName, char* Workplace, char* Email, char* Telegram, char* Instagramm);
void ViewContact(Person contactlist[], int cnt);

#endif
