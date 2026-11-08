CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_GNU_SOURCE

TARGET = priority
OBJS = priority.o priorityfunc.o
TEST = test_priority
TESTOBJS = test_priority.o priorityfunc.o

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(TEST): $(TESTOBJS)
	$(CC) $(CFLAGS) -o $@ $(TESTOBJS)

test: $(TEST)
	./$(TEST)

%.o: %.c priority.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) test_priority.o $(TARGET) $(TEST)
