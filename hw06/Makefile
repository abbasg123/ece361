CC = gcc
CPPFLAGS = -I.
CFLAGS = -std=c11 -Wall -Wextra
LDLIBS = -lm

OBJECTS = main.o sensor.o panel.o scenario.o iom361_r4.o
TEST_OBJECTS = tests/test_hw06.o sensor.o panel.o scenario.o iom361_r4.o

.PHONY: all test clean

all: replay

replay: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

tests/test_hw06: $(TEST_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(TEST_OBJECTS) $(LDLIBS)

test: replay tests/test_hw06
	./tests/test_hw06
	sh ./regress.sh

main.o: main.c sensor.h panel.h scenario.h iom361_r4.h
sensor.o: sensor.c sensor.h iom361_r4.h
panel.o: panel.c panel.h iom361_r4.h
scenario.o: scenario.c scenario.h
iom361_r4.o: iom361_r4.c iom361_r4.h
tests/test_hw06.o: tests/test_hw06.c sensor.h panel.h scenario.h iom361_r4.h

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) tests/test_hw06.o tests/test_hw06 replay
