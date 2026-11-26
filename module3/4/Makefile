CC = gcc
CFLAGS = -D_DEFAULT_SOURCE -std=c11 -Wall -Wextra -Wpedantic -O2

all: task4_sysv

task4_sysv: task4_sysv.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f task4_sysv

.PHONY: all clean
