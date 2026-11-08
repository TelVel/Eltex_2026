CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_GNU_SOURCE

TARGET = chmod
OBJS = chmod.o chmodfunc.o
TEST = test_chmod
TESTOBJS = test_chmod.o chmodfunc.o

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(TEST): $(TESTOBJS)
	$(CC) $(CFLAGS) -o $@ $(TESTOBJS)

test: $(TEST)
	./$(TEST)

%.o: %.c chmod.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) test_chmod.o $(TARGET) $(TEST)
