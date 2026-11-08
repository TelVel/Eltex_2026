#include <stdio.h>
#include "iptest.h"

static int failed;

static void check(int cond, const char *msg) {
    if (!cond) {
        printf("FAIL: %s\n", msg);
        failed++;
    }
}

int main(void) {
    uint32_t host = ipToUint("192.168.1.1");
    uint32_t other = ipToUint("192.168.2.15");
    uint32_t foreign = ipToUint("10.0.0.1");
    uint32_t mask16 = ipToUint("255.255.0.0");
    uint32_t mask24 = ipToUint("255.255.255.0");
    uint32_t mask8 = ipToUint("255.0.0.0");

    printf("192.168.1.1 -> %u\n", host);
    check(host == 0xC0A80101u, "192.168.1.1");
    printf("0.0.0.0 -> %u\n", ipToUint("0.0.0.0"));
    check(ipToUint("0.0.0.0") == 0, "zero address");
    printf("255.255.255.255 -> %u\n", ipToUint("255.255.255.255"));
    check(ipToUint("255.255.255.255") == 0xFFFFFFFFu, "broadcast");
    printf("255.255.255.0 -> %u\n", mask24);
    check(mask24 == 0xFFFFFF00u, "24-bit mask");

    printf("192.168.1.1 in 192.168.1.1/24 -> %s\n", SubnetTest(host, host, mask24) ? "yes" : "no");
    check(SubnetTest(host, host, mask24) == 1, "host is in its own /24");
    printf("192.168.2.15 in 192.168.1.1/24 -> %s\n", SubnetTest(other, host, mask24) ? "yes" : "no");
    check(SubnetTest(other, host, mask24) == 0, "other host is outside /24");
    printf("192.168.2.15 in 192.168.1.1/16 -> %s\n", SubnetTest(other, host, mask16) ? "yes" : "no");
    check(SubnetTest(other, host, mask16) == 1, "other host is inside /16");
    printf("10.0.0.1 in 192.168.1.1/8 -> %s\n", SubnetTest(foreign, host, mask8) ? "yes" : "no");
    check(SubnetTest(foreign, host, mask8) == 0, "10.0.0.1 is outside 192/8");
    printf("10.0.0.1 in 192.168.1.1/0 -> %s\n", SubnetTest(foreign, host, 0) ? "yes" : "no");
    check(SubnetTest(foreign, host, 0) == 1, "zero mask matches everything");

    if (failed) {
        printf("%d test(s) failed\n", failed);
        return 1;
    }
    printf("all tests passed\n");
    return 0;
}
