#include <stdio.h>
#include <string.h>
#include "chmod.h"

static int failed;

static void check(int cond, const char *msg) {
    if (!cond) {
        printf("FAIL: %s\n", msg);
        failed++;
    }
}

int main(void) {
    char bits[10];
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    IntToBin(431, bits);
    printf("431 -> %s\n", bits);
    check(strcmp(bits, "100011001") == 0, "431 to binary");
    IntToBin(755, bits);
    printf("755 -> %s\n", bits);
    check(strcmp(bits, "111101101") == 0, "755 to binary");
    IntToBin(0, bits);
    printf("0 -> %s\n", bits);
    check(strcmp(bits, "000000000") == 0, "0 to binary");

    CharToBin("rwxr-xr-x", bits);
    printf("rwxr-xr-x -> %s\n", bits);
    check(strcmp(bits, "111101101") == 0, "rwx string to binary");
    CharToBin("---------", bits);
    printf("--------- -> %s\n", bits);
    check(strcmp(bits, "000000000") == 0, "empty string to binary");

    PermToString(mode, bits);
    printf("644 string -> %s\n", bits);
    check(strcmp(bits, "rw-r--r--") == 0, "mode to string");
    PermToBin(mode, bits);
    printf("644 binary -> %s\n", bits);
    check(strcmp(bits, "110100100") == 0, "mode to binary");
    printf("644 number -> %d\n", PermToInt(mode));
    check(PermToInt(mode) == 644, "mode to octal digits");

    mode = ModifyPerm(mode, "u+x");
    PermToString(mode, bits);
    printf("u+x -> %s (%d)\n", bits, PermToInt(mode));
    check(PermToInt(mode) == 744, "u+x");
    check(strcmp(bits, "rwxr--r--") == 0, "u+x string");

    mode = ModifyPerm(mode, "a+x");
    PermToString(mode, bits);
    printf("a+x -> %s (%d)\n", bits, PermToInt(mode));
    check(PermToInt(mode) == 755, "a+x");

    mode = ModifyPerm(mode, "g-r");
    PermToString(mode, bits);
    printf("g-r -> %s (%d)\n", bits, PermToInt(mode));
    check(PermToInt(mode) == 715, "g-r");

    mode = ModifyPerm(mode, "a-rwx");
    PermToString(mode, bits);
    printf("a-rwx -> %s (%d)\n", bits, PermToInt(mode));
    check(PermToInt(mode) == 0, "a-rwx");
    check(strcmp(bits, "---------") == 0, "cleared string");

    mode = ModifyPerm(0, "o+rw");
    PermToString(mode, bits);
    printf("o+rw -> %s (%d)\n", bits, PermToInt(mode));
    check(PermToInt(mode) == 6, "o+rw");

    if (failed) {
        printf("%d test(s) failed\n", failed);
        return 1;
    }
    printf("all tests passed\n");
    return 0;
}
