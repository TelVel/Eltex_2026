CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_GNU_SOURCE

TARGET = iptest
OBJS = iptest.o iptestfunc.o
TEST = test_iptest
TESTOBJS = test_iptest.o iptestfunc.o

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(TEST): $(TESTOBJS)
	$(CC) $(CFLAGS) -o $@ $(TESTOBJS)

test: $(TEST)
	./$(TEST)

%.o: %.c iptest.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) test_iptest.o $(TARGET) $(TEST)
