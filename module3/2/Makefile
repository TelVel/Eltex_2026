CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2

all: broker

broker: broker.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f broker

.PHONY: all clean
