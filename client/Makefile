CC=clang
CFLAGS=-c -Wall
LDFLAGS=-lm
SOURCES=main.c client.c util.c command.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=client

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o client
