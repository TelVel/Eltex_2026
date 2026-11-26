CC = gcc
CFLAGS = -D_DEFAULT_SOURCE -std=c11 -Wall -Wextra -Wpedantic -O2

all: task5_posix

task5_posix: task5_posix.c
	$(CC) $(CFLAGS) -o $@ $< -pthread -lrt

clean:
	rm -f task5_posix

.PHONY: all clean
