CC = gcc
CFLAGS = -std=c11 -Wall -Wextra

TARGET = door
SOURCES = main.c sensor.c fsm.c store.c aux.c report.c iom361_r4.c
OBJECTS = $(SOURCES:.c=.o)
DEPS = $(SOURCES:.c=.d)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(TARGET) $(OBJECTS) $(DEPS)
