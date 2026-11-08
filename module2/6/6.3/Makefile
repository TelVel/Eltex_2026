CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_GNU_SOURCE -fPIC
LDLIBS = -ldl -lm

TARGET = calcimproved
PLUGINS = lib/libsum.so lib/libsub.so lib/libmult.so lib/libdiv.so lib/libpower.so
TEST = test_calc

.PHONY: all clean test

all: $(TARGET)

$(TARGET): calcimproved.c calc.h $(PLUGINS)
	$(CC) $(CFLAGS) -o $@ calcimproved.c $(LDLIBS)

lib/libsum.so: calcsum.c calc.h
	mkdir -p lib
	$(CC) $(CFLAGS) -shared -o $@ calcsum.c

lib/libsub.so: calcsub.c calc.h
	mkdir -p lib
	$(CC) $(CFLAGS) -shared -o $@ calcsub.c

lib/libmult.so: calcmult.c calc.h
	mkdir -p lib
	$(CC) $(CFLAGS) -shared -o $@ calcmult.c

lib/libdiv.so: calcdiv.c calc.h
	mkdir -p lib
	$(CC) $(CFLAGS) -shared -o $@ calcdiv.c -lm

lib/libpower.so: calcpower.c calc.h
	mkdir -p lib
	$(CC) $(CFLAGS) -shared -o $@ calcpower.c -lm

$(TEST): test_calc.c calc.h $(PLUGINS) calcsum.c calcsub.c calcmult.c calcdiv.c calcpower.c
	$(CC) $(CFLAGS) -o $@ test_calc.c calcsum.c calcsub.c calcmult.c calcdiv.c calcpower.c $(LDLIBS)

test: $(TEST)
	./$(TEST)

clean:
	rm -f $(TARGET) $(TEST) $(PLUGINS)
	rmdir lib 2>/dev/null || true
