CC      = gcc
CFLAGS  = -Wall -Wextra -O2

all: copy

copy: copy.c
	$(CC) $(CFLAGS) -o $@ $<

testscript: copy
	./test.sh
	
testy: copy
	echo hello > f1.txt
	echo hello,bin > f2.bin
	./copy f1.txt
	./copy f2.bin
clean:
	rm -f copy *.copy test_fifo
	rm -f f1.txt f2.bin f3.txt a.txt b.txt

.PHONY: all test clean testy