CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g

.PHONY: all test clean

all: lock

lock: main.o lock.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ main.o lock.o

main.o: main.c lock.h
	$(CC) $(CFLAGS) -c main.c -o $@

lock.o: lock.c lock.h
	$(CC) $(CFLAGS) -c lock.c -o $@

tests/test_lock.o: tests/test_lock.c lock.h
	$(CC) $(CFLAGS) -c tests/test_lock.c -o $@

tests/test_lock: tests/test_lock.o lock.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ tests/test_lock.o lock.o

test: tests/test_lock
	./tests/test_lock

# Remove only executables and object files produced by these rules.
clean:
	$(RM) lock tests/test_lock main.o lock.o tests/test_lock.o
