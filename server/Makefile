CC=gcc
CFLAGS=-c -Wall
LDFLAGS=-lm
SOURCES=server.c reply.c fdlist.c command.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ftpserver

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ -lpthread

clean:
	rm -rf *.o ftpserver
