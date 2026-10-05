CC = gcc
CFLAGS = -std=c11 -Wall -Wextra
CPPFLAGS = -I.

OBJECTS = bits.o status.o tests/test_bits.o
TEST_PROGRAM = tests/test_bits

.PHONY: all test clean

all: $(OBJECTS)

bits.o: bits.c bits.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c bits.c -o bits.o

status.o: status.c status.h bits.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c status.c -o status.o

tests/test_bits.o: tests/test_bits.c bits.h status.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c tests/test_bits.c -o tests/test_bits.o

$(TEST_PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TEST_PROGRAM)

test: $(TEST_PROGRAM)
	./$(TEST_PROGRAM)

clean:
	rm -f $(OBJECTS) $(TEST_PROGRAM)