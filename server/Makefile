CC=gcc
CFLAGS=-c -Wall
LDFLAGS=-lm
SOURCES=server.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ftpserver

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o ftpserver
