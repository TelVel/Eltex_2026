CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2

all: chat

chat: chat.c
	$(CC) $(CFLAGS) -o $@ $< -lrt $(LDFLAGS)

clean:
	rm -f chat

.PHONY: all clean
