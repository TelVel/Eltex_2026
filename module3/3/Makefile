CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2

all: task3_chat

task3_chat: task3_chat.c
	$(CC) $(CFLAGS) -o $@ $< -lrt $(LDFLAGS)

clean:
	rm -f task2_broker task3_chat

.PHONY: all clean
