CC = gcc
CPPFLAGS = -I.
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g
SANFLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all

.PHONY: all test memcheck clean

all: treestats tests/test_bst

treestats: treestats.c bst.c bst.h
	$(CC) $(CPPFLAGS) $(CFLAGS) treestats.c bst.c $(LDFLAGS) $(LDLIBS) -o $@

tests/test_bst: tests/test_bst.c bst.c bst.h
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_bst.c bst.c $(LDFLAGS) $(LDLIBS) -o $@

test: tests/test_bst
	./tests/test_bst

tests/test_bst_memcheck: tests/test_bst.c bst.c bst.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SANFLAGS) tests/test_bst.c bst.c $(LDFLAGS) $(LDLIBS) -o $@

memcheck: tests/test_bst_memcheck
	./tests/test_bst_memcheck

clean:
	rm -f treestats tests/test_bst tests/test_bst_memcheck
