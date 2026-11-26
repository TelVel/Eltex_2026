CC = gcc
CFLAGS = -D_DEFAULT_SOURCE -std=c11 -Wall -Wextra -Wpedantic -O2

all: posix

posix: posix.c
	$(CC) $(CFLAGS) -o $@ $< -pthread -lrt

clean:
	rm -f posix

.PHONY: all clean
