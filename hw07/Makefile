CC = cc
CPPFLAGS += -I.
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g -O2
LDFLAGS =
LDLIBS =

SAN_CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g -O1 \
             -fsanitize=address,undefined -fno-omit-frame-pointer \
             -fno-sanitize-recover=all

.PHONY: all test memcheck clean

all: balance

balance: balance.o istack.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

tests/test_istack: tests/test_istack.o istack.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

balance.o: balance.c istack.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

istack.o: istack.c istack.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

tests/test_istack.o: tests/test_istack.c istack.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

test: balance tests/test_istack
	./tests/test_istack
	./regress.sh

# Separate recursive makes ensure cleaning finishes before sanitized builds.
memcheck:
	$(MAKE) clean
	$(MAKE) CFLAGS='$(SAN_CFLAGS)' LDFLAGS='-fsanitize=address,undefined' test

clean:
	rm -f balance balance.o istack.o tests/test_istack tests/test_istack.o
