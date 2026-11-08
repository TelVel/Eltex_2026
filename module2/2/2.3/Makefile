CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_GNU_SOURCE
LDLIBS = -lm

TARGET = calcimproved
OBJS = calcimproved.o calcimprovedfunc.o
TEST = test_calcimproved
TESTOBJS = test_calcimproved.o calcimprovedfunc.o

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(TEST): $(TESTOBJS)
	$(CC) $(CFLAGS) -o $@ $(TESTOBJS) $(LDLIBS)

test: $(TEST)
	./$(TEST)

%.o: %.c calcimproved.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) test_calcimproved.o $(TARGET) $(TEST)
