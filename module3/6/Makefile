CC = gcc
CFLAGS = -D_DEFAULT_SOURCE -std=c11 -Wall -Wextra -Wpedantic -O2

all: udp

udp: udp.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f udp

.PHONY: all clean
