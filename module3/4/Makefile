CC = gcc
CFLAGS = -D_DEFAULT_SOURCE -std=c11 -Wall -Wextra -Wpedantic -O2

all: sysv

sysv: sysv.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f sysv

.PHONY: all clean
