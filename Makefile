CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = charge_server

SOURCES = main.c network.c protocol.c session.c utils.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = network.h protocol.h session.h utils.h

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
