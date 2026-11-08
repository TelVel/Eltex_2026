#include <math.h>
#include <stdio.h>
#include "calcimproved.h"

static int failed;

static void check(int cond, const char *msg) {
    if (!cond) {
        printf("FAIL: %s\n", msg);
        failed++;
    }
}

static int near(float got, float expect) {
    return fabsf(got - expect) < 0.0001f;
}

static void show(const char *op, float a, float b, float result, float expect, int infinite) {
    printf("%g %s %g = %f\n", a, op, b, result);
    if (infinite) check(isinf(result), op);
    else check(near(result, expect), op);
}

int main(void) {
    float (*operations[])(float, float) = {sum, sub, mult, div, powa};
    const char *ops[] = {"+", "-", "*", "/", "^"};

    show(ops[0], 1.5f, 2.5f, operations[0](1.5f, 2.5f), 4.0f, 0);
    show(ops[1], 5.0f, 1.5f, operations[1](5.0f, 1.5f), 3.5f, 0);
    show(ops[2], -2.0f, 4.0f, operations[2](-2.0f, 4.0f), -8.0f, 0);
    show(ops[3], 9.0f, 3.0f, operations[3](9.0f, 3.0f), 3.0f, 0);
    show(ops[3], 1.0f, 0.0f, operations[3](1.0f, 0.0f), 0.0f, 1);
    show(ops[4], 2.0f, 10.0f, operations[4](2.0f, 10.0f), 1024.0f, 0);
    check(sizeof(operations) / sizeof(operations[0]) == 5, "five operations");

    if (failed) {
        printf("%d test(s) failed\n", failed);
        return 1;
    }
    printf("all tests passed\n");
    return 0;
}
