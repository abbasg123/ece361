CC = gcc
CFLAGS = -std=c11 -Wall -Wextra

.PHONY: all test clean

all: readings

readings: main.o reading.o stats.o histogram.o
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c readings.h stats.h histogram.h
	$(CC) $(CFLAGS) -c $< -o $@

reading.o: readings.c readings.h
	$(CC) $(CFLAGS) -c $< -o $@

stats.o: stats.c stats.h
	$(CC) $(CFLAGS) -c $< -o $@

histogram.o: histogram.c histogram.h
	$(CC) $(CFLAGS) -c $< -o $@

tests/test_stats.o: tests/test_stats.c stats.h
	$(CC) $(CFLAGS) -c $< -o $@

tests/test_stats: tests/test_stats.o stats.o
	$(CC) $(CFLAGS) -o $@ $^

test: readings tests/test_stats
	./tests/test_stats
	sh regress.sh

clean:
	rm -f readings main.o reading.o stats.o histogram.o tests/test_stats tests/test_stats.o
