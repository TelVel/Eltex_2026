#ifndef IPTEST_H
#define IPTEST_H

#include <stdint.h>

uint32_t ipToUint(const char *ip);
int SubnetTest(uint32_t ip, uint32_t subnet, uint32_t mask);

#endif
