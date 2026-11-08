CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_GNU_SOURCE

TARGET = logsorted
OBJS = logsorted.o logsortedfunc.o
TEST = test_logsorted
TESTOBJS = test_logsorted.o logsortedfunc.o

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(TEST): $(TESTOBJS)
	$(CC) $(CFLAGS) -o $@ $(TESTOBJS)

test: $(TEST)
	./$(TEST)

%.o: %.c logsorted.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) test_logsorted.o $(TARGET) $(TEST)
