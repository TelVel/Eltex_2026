#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "chmod.h"

int main(int argc, char *argv[]) {
    char perm[10];
    char *ptr = argv[1];
    long num;
    num = strtol(ptr, &ptr, 10);
    if (argc == 2) {
        if (*ptr == '\0') {
            char binary_int[10];
            int UB = atoi(argv[1]);
            IntToBin(UB, binary_int);
            printf("%s \n", binary_int);
        } else {
            if (strlen(argv[1]) != 9) {
                struct stat buffer;
                const char *filename = argv[1];
                stat(filename, &buffer);
                PermToString(buffer.st_mode, perm);
                printf("%s\n", perm);
                PermToBin(buffer.st_mode, perm);
                printf("%s\n", perm);
                printf("%d\n", PermToInt(buffer.st_mode));
            } else {
                char binary_char[10];
                const char *perm_ch = argv[1];
                CharToBin(perm_ch, binary_char);
                printf("%s \n", binary_char);
            }
        }
    }
    char command[10];
    if (argc == 3) {
        struct stat buffer;
        mode_t mode;
        const char *filename = argv[2];
        stat(filename, &buffer);
        PermToString(buffer.st_mode, perm);
        printf("Default string permissions: %s\n", perm);
        PermToBin(buffer.st_mode, perm);
        printf("Default binary permissions: %s\n", perm);
        printf("Default number permissions: %d\n", PermToInt(buffer.st_mode));
        const char *command = argv[1];
        mode = ModifyPerm(buffer.st_mode, command);
        PermToString(mode, perm);
        printf("Change string permissions: %s\n", perm);
        PermToBin(mode, perm);
        printf("Change binary permissions: %s\n", perm);
        printf("Change number permissions: %d\n", PermToInt(mode));
    }
}
