#ifndef LOGTREE_H
#define LOGTREE_H

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
    struct Person* left;
    struct Person* right;
    int height;
} Person;

int max(int a, int b);
int height(Person* node);
Person* newNode(int id, char* LastName, char* FirstName, char* MiddleName, char* Workplace, char* Email, char* Telegram, char* Instagramm);
Person* rightRotate(Person* y);
Person* leftRotate(Person* x);
int getBalance(Person* node);
Person* insert(Person* node, int id, char* LastName, char* FirstName, char* MiddleName, char* Workplace, char* Email, char* Telegram, char* Instagramm);
Person* minValueNode(Person* node);
Person* deleteNode(Person* root, int id);
void preOrder(Person* root);
void printTree(Person* root, int space);

#endif
