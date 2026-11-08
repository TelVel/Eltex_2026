#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "iptest.h"

int main(int argc, char *argv[]) {
    char *ip = argv[1];
    char *mask = argv[2];
    int N = atoi(argv[3]);
    float cnt = 0;
    srand(time(NULL));
    uint32_t iptemp;
    uint32_t subnet = ipToUint(ip);
    uint32_t maskuint = ipToUint(mask);
    (void)argc;
    for (int i = 0; i < N; i++) {
        for (int j = 4; j > 0; --j) {
            uint8_t gquad = (uint8_t)(rand() % 255);
            iptemp = (iptemp << 8) | gquad;
        }
        cnt += SubnetTest(iptemp, subnet, maskuint);
    }
    printf("Subnet address count: %f    ", cnt);
    printf("Subnet address percent: %f %%\n", (cnt / N) * 100);
    printf("Non-Subnet address count: %f    ", N - cnt);
    printf("Non-Subnet address percent: %f %%\n", ((N - cnt) / N) * 100);
}
