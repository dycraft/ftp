CC=clang
CFLAGS=-c -Wall
LDFLAGS=-lm
SOURCES=client.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ftpclient

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o ftpclient
