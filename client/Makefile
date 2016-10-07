CC=gcc
CFLAGS=-c -Wall
LDFLAGS=-lm
SOURCES=client.c util.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ftpclient

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o ftpclient
