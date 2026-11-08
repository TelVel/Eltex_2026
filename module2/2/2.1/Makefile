CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_GNU_SOURCE

TARGET = mainlog
OBJS = mainlog.o mainlogfunc.o
TEST = test_mainlog
TESTOBJS = test_mainlog.o mainlogfunc.o

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(TEST): $(TESTOBJS)
	$(CC) $(CFLAGS) -o $@ $(TESTOBJS)

test: $(TEST)
	./$(TEST)

%.o: %.c mainlog.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) test_mainlog.o $(TARGET) $(TEST)
