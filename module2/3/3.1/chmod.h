#ifndef CHMOD_H
#define CHMOD_H

#include <sys/stat.h>

void IntToBin(int num, char *binary_int);
void CharToBin(const char *perm, char *binary_ch);
void PermToString(mode_t mode, char *permission);
void PermToBin(mode_t mode, char *permission);
int PermToInt(mode_t mode);
mode_t ModifyPerm(mode_t mode, const char *command);

#endif
