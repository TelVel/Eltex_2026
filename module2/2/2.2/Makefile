CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_GNU_SOURCE
LDLIBS = -lm

TARGET = calcbasic
OBJS = calcbasic.o calcbasicfunc.o
TEST = test_calcbasic
TESTOBJS = test_calcbasic.o calcbasicfunc.o

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(TEST): $(TESTOBJS)
	$(CC) $(CFLAGS) -o $@ $(TESTOBJS) $(LDLIBS)

test: $(TEST)
	./$(TEST)

%.o: %.c calcbasic.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) test_calcbasic.o $(TARGET) $(TEST)
