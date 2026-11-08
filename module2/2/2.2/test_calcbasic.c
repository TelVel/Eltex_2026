#include <math.h>
#include <stdio.h>
#include "calcbasic.h"

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
    show("+", 1.5f, 2.5f, sum(1.5f, 2.5f), 4.0f, 0);
    show("-", 5.0f, 1.5f, sub(5.0f, 1.5f), 3.5f, 0);
    show("*", 3.0f, 4.0f, mult(3.0f, 4.0f), 12.0f, 0);
    show("/", 7.0f, 2.0f, div(7.0f, 2.0f), 3.5f, 0);
    show("/", 1.0f, 0.0f, div(1.0f, 0.0f), 0.0f, 1);
    show("^", 2.0f, 3.0f, powa(2.0f, 3.0f), 8.0f, 0);
    show("^", 9.0f, 0.5f, powa(9.0f, 0.5f), 3.0f, 0);

    if (failed) {
        printf("%d test(s) failed\n", failed);
        return 1;
    }
    printf("all tests passed\n");
    return 0;
}
