CC = gcc
CFLAGS = -D_DEFAULT_SOURCE -std=c11 -Wall -Wextra -Wpedantic -O2

all: task6_udp

task6_udp: task6_udp.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f task6_udp

.PHONY: all clean
