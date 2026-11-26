CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2

all: task2_broker

task2_broker: task2_broker.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f task2_broker

.PHONY: all clean
