CC = gcc
CFLAGS = -std=c11 -Wall -Wextra

all: ptr.o tests/test_ptr

ptr.o: ptr.c ptr.h
	$(CC) $(CFLAGS) -c ptr.c -o ptr.o

tests/test_ptr: tests/test_ptr.c ptr.h ptr.o
	$(CC) $(CFLAGS) tests/test_ptr.c ptr.o -o tests/test_ptr

test: tests/test_ptr
	./tests/test_ptr

clean:
	rm -f ptr.o tests/test_ptr

.PHONY: all test clean
