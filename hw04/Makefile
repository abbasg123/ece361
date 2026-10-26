CC = gcc
CFLAGS = -std=c11 -Wall -Wextra

.PHONY: all test clean


all: events

events: events.o tally.o
	$(CC) $(CFLAGS) -o $@ $^


events.o: events.c tally.h
	$(CC) $(CFLAGS) -c $< -o $@

tally.o: tally.c tally.h
	$(CC) $(CFLAGS) -c $< -o $@

tests/test_tally.o: tests/test_tally.c tally.h
	$(CC) $(CFLAGS) -c $< -o $@

tests/test_tally: tests/test_tally.o tally.o
	$(CC) $(CFLAGS) -o $@ $^

test: tests/test_tally
	./tests/test_tally

clean:
	rm -f events events.o tally.o tests/test_tally tests/test_tally.o
