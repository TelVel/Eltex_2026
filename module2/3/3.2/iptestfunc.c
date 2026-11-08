#include <stdlib.h>
#include <string.h>
#include "iptest.h"

uint32_t ipToUint(const char *ip)
{
    uint32_t value = 0;
    uint8_t quad;
    char *ipc = strdup(ip);
    char *piece = strtok(ipc, ".");
    for (int i = 0; i < 4; ++i) {
        quad = (uint8_t)atoi(piece);
        value = (value << 8) | quad;
        piece = strtok(NULL, ".");
    }
    free(ipc);
    return value;
}

int SubnetTest(uint32_t ip, uint32_t subnet, uint32_t mask) {
    return ((subnet & mask) == (ip & mask)) ? 1 : 0;
}
