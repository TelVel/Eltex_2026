#include <dlfcn.h>
#include <math.h>
#include <stdio.h>
#include "calc.h"

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

static void check_plugin(const char *path, const char *symbol, const char *op, float a, float b, float expect, int infinite) {
    void *handle = dlopen(path, RTLD_LAZY);
    float (*fn)(float, float);
    float result;

    if (handle == NULL) {
        printf("FAIL: dlopen %s: %s\n", path, dlerror());
        failed++;
        return;
    }
    fn = dlsym(handle, symbol);
    if (fn == NULL) {
        printf("FAIL: dlsym %s: %s\n", symbol, dlerror());
        failed++;
        dlclose(handle);
        return;
    }
    result = fn(a, b);
    printf("%s: ", path);
    show(op, a, b, result, expect, infinite);
    dlclose(handle);
}

int main(void) {
    show("+", 1.5f, 2.5f, sum(1.5f, 2.5f), 4.0f, 0);
    show("-", 5.0f, 1.5f, sub(5.0f, 1.5f), 3.5f, 0);
    show("*", 3.0f, -4.0f, mult(3.0f, -4.0f), -12.0f, 0);
    show("/", 7.0f, 2.0f, div(7.0f, 2.0f), 3.5f, 0);
    show("/", 1.0f, 0.0f, div(1.0f, 0.0f), 0.0f, 1);
    show("^", 2.0f, 3.0f, powa(2.0f, 3.0f), 8.0f, 0);

    check_plugin("lib/libsum.so", "sum", "+", 2.0f, 2.0f, 4.0f, 0);
    check_plugin("lib/libsub.so", "sub", "-", 2.0f, 5.0f, -3.0f, 0);
    check_plugin("lib/libmult.so", "mult", "*", 6.0f, 7.0f, 42.0f, 0);
    check_plugin("lib/libdiv.so", "div", "/", 1.0f, 0.0f, 0.0f, 1);
    check_plugin("lib/libpower.so", "powa", "^", 3.0f, 2.0f, 9.0f, 0);

    if (failed) {
        printf("%d test(s) failed\n", failed);
        return 1;
    }
    printf("all tests passed\n");
    return 0;
}
